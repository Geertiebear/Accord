#include <accordserver/thread/Thread.h>

#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <system_error>
#include <event2/bufferevent_ssl.h>

#include <accordserver/Server.h>
#include <accordserver/database/Database.h>
#include <accordserver/Authentication.h>
#include <accordserver/log/Logger.h>
#include <accordserver/util/LibEventUtil.h>
#include <accordshared/error/ErrorCodes.h>
#include <accordshared/network/PacketDecoder.h>
#include <accordshared/network/packet/ErrorPacket.h>
#include <accordshared/network/packet/SerializationPacket.h>
#include <accordshared/network/packet/DisconnectPacket.h>
#include <accordshared/util/BinUtil.h>
#include <accordserver/util/CryptoUtil.h>
#include <accordshared/types/Database.h>

namespace accord {
namespace thread {

Thread::Thread(Server &server, const Config &config) : server(server),
    config(config), thread(),
    database(config.database)
{
	eventBase = event_base_new();
}

Thread::~Thread()
{
	struct timeval timeout {
	    .tv_sec = 5,
	    .tv_usec = 0,
	};
	event_base_loopexit(eventBase, &timeout);
    if (thread.joinable())
        thread.join();
}

void Thread::run()
{
	event_base_loop(eventBase, EVLOOP_NO_EXIT_ON_EMPTY);
}

void Thread::stop()
{
    this->~Thread();
}

void Thread::start()
{
    database.connect();
    thread = std::thread(&Thread::run, this);
    try {
        thread.detach();
    } catch (std::system_error &e) {
		log::Logger::log(log::ERROR, "Error detatching thread!");
    }
}

void Thread::acceptClient(evutil_socket_t clientSocket, SSL *ssl)
{
    /*
     * we defer our callbacks because the bufferevent could be
     * emptied before we have a chance to copy over the buffer
     * the the writeBuffer. Which results in the write callback
     * being called without the buffer having been copied, wasting
     * a callback and having to wait for a new one. Deffering the callback
     * makes sure that the buffer has been copied before the callback
     * gets called.
     */
	struct bufferevent *bufferEvent = bufferevent_openssl_socket_new(eventBase,
            clientSocket, ssl, BUFFEREVENT_SSL_OPEN, BEV_OPT_THREADSAFE |
                          BEV_OPT_DEFER_CALLBACKS);
    //TODO: move this to constructor
    Client *client = new Client(server, *this);
    client->channel = 0;
	client->bufferEvent = bufferEvent;
    client->hasPartialPacket = false;
    client->writeBuffer.clear();

    bufferevent_setcb(bufferEvent, &Thread::readCallback,
                      &Thread::writeCallback,
			&Thread::eventCallback, client);
	bufferevent_enable(bufferEvent, EV_READ | EV_WRITE);
    clients.push_back(client);
}

void Thread::removeClient(Client *client)
{
    client->markOffline();
	auto it = std::find(clients.begin(), clients.end(), client);
	clients.erase(it);
	util::LibEventUtil::freeBufferEvent(client->bufferEvent);
	delete client;
}

void Thread::broadcast(const std::vector<char> &data)
{
	for (Client *client : clients) {
			bufferevent_lock(client->bufferEvent);
            client->write(data);
            bufferevent_unlock(client->bufferEvent);
	}
}

int Thread::isInviteValid(const std::string &invite)
{
    /*
     * this is a thread function since we first check the cache and
     * afterwards check it in main database to see if it has
     * expired. The invite should always be in the cache if it is
     * valid but this may not always be the case.
     */
    if (server.isInviteValid(invite))
        return 1;
    return checkInviteInDatabase(invite);
}

uint64_t Thread::getCommunityForInvite(const std::string &invite)
{
    return server.getCommunityForInvite(invite);
}

std::string Thread::genInvite(uint64_t communityId)
{
    auto invite = util::CryptoUtil::getRandomString(INVITE_LENGTH);
    server.insertInvite(communityId, invite);
    /* TODO: also insert into db */
    return invite;
}

void Thread::readCallback(struct bufferevent *bufferEvent, void *data)
{
    log::Logger::log(log::DEBUG, "readCallback()");
    Client *client = (Client*) data;
    if (client->hasPartialPacket) {
        handlePartialPacket(bufferEvent, client);
        return;
    }

    std::vector<char> buffer;
    buffer.resize(HEADER_SIZE);
    bufferevent_read(bufferEvent, &buffer[0], HEADER_SIZE);

    /*
    * first two bytes are packet id
    * last 8 bytes are the packet length
    * grab the packet length and id and see
    * if we can receive the entire packet in one go.
    * If we can't we save the ID and length in the
    * client and empty the buffer in there. Then next iteration
    * we repeat the process
    * If we can that's fine we pass the ID and buffer onto
    * the packet decoder.
    */
    const auto id = util::BinUtil::assembleUint16(buffer[0], buffer[1]);
    const auto length = util::BinUtil::assembleUint64(
                std::vector<uint8_t>(buffer.begin() + 2, buffer.end()));
    const auto bufferSize = evbuffer_get_length(bufferevent_get_input(
                                                  bufferEvent));

    if (length > bufferSize) {
        log::Logger::log(log::DEBUG, "We have identified it is a partial packet!");
        client->partialPacket.length = length;
        client->partialPacket.id = id;
        client->partialPacket.buffer.resize(bufferSize);
        bufferevent_read(bufferEvent, &client->partialPacket.buffer[0],
                bufferSize);
        client->hasPartialPacket = true;
        return;
    }

    buffer.resize(length);
    bufferevent_read(bufferEvent, &buffer[0], length);
    int ret = network::PacketDecoder::receivePacket(id, buffer, (PacketData*) data);
    if (ret != 1)
        dispatchError(client, (Error) ret);
}

void Thread::eventCallback(struct bufferevent *bufferEvent, short events,
		void *data)
{
    if (events & BEV_EVENT_ERROR) {
        log::Logger::log(log::DEBUG, "There has been an error "
                                     "with a client! Closing the "
                                     "connection...");
        Client *client = (Client*) data;
        client->thread.removeClient(client);
	}
}

void Thread::handlePartialPacket(bufferevent *bufferEvent, Client *client)
{
    log::Logger::log(log::DEBUG, "Entering handlePartialPacket()!");
    const auto &length = client->partialPacket.length;
    const auto &id = client->partialPacket.id;
    auto &buffer = client->partialPacket.buffer;
    auto currentBufferSize = buffer.size();
    const uint64_t bufferSize = evbuffer_get_length(bufferevent_get_input(
                                                  bufferEvent));
    const auto toRead = std::min(length - currentBufferSize, bufferSize);

    buffer.resize(toRead + currentBufferSize);
    bufferevent_read(bufferEvent,
                     &buffer[currentBufferSize],
                     toRead);

    currentBufferSize = buffer.size();
    if (length > currentBufferSize)
        return;

    client->hasPartialPacket = false;
    int ret = network::PacketDecoder::receivePacket(id, buffer, (PacketData*) client);
    if (ret != 1)
        dispatchError(client, (Error) ret);
}

void Thread::writeCallback(struct bufferevent *bufferEvent, void *data)
{
    Client *client = (Client*) data;
    auto &buffer = client->writeBuffer;

    if (buffer.empty())
        return;

    const auto &size = buffer.size();
    const auto evBufferSize = evbuffer_get_length(bufferevent_get_output(
                                                      bufferEvent));
    bufferevent_write(bufferEvent, &buffer[0], size);
    const auto evBufferSize2 = evbuffer_get_length(bufferevent_get_output(
                                                       bufferEvent));
    const uint64_t written = evBufferSize2 - evBufferSize;
    buffer.erase(buffer.begin(), buffer.begin() + written);
}

void Thread::dispatchError(Client *client, Error error)
{
    network::ErrorPacket packet;
    const auto msg = packet.construct(error);
    client->write(msg);
}

int Thread::checkInviteInDatabase(const std::string &invite)
{
    return 0; /* TODO */
}

/* start client functions */
void Client::write(const std::vector<char> &msg)
{
    const auto size = msg.size();
    const auto evBufferSize = evbuffer_get_length(bufferevent_get_output(
                                                      bufferEvent));
    bufferevent_write(bufferEvent, &msg[0], size);
    const auto evBufferSize2 = evbuffer_get_length(bufferevent_get_output(
                                                       bufferEvent));
    const uint64_t written = evBufferSize2 - evBufferSize;

    if (size > written)
        std::copy(msg.begin() + written, msg.end(), std::back_inserter(
                      writeBuffer));
}

void Client::markOffline()
{
    for (uint64_t channel : channelList)
        server.removeOnlineMember(channel, user.id(), this);
    server.notifyStatusChange(user.id(), this);
}

} /* namespace thread */
} /* namespace auth */
