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
#include <accordshared/util/BinUtil.h>
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
	struct bufferevent *bufferEvent = bufferevent_openssl_socket_new(eventBase,
			clientSocket, ssl, BUFFEREVENT_SSL_OPEN, BEV_OPT_THREADSAFE);
    struct timeval readTimeout = {
        .tv_sec = 30,
        .tv_usec = 0,
    };

    Client *client = new Client(server, *this);
    client->channel = 0;
	client->bufferEvent = bufferEvent;

	bufferevent_setcb(bufferEvent, &Thread::readCallback, NULL,
			&Thread::eventCallback, client);
	bufferevent_set_timeouts(bufferEvent, &readTimeout, NULL);
	bufferevent_enable(bufferEvent, EV_READ | EV_WRITE);
    clients.push_back(client);

    //TODO: temp, please remove if it's still here
    network::SerializationPacket packet;
    types::CommunitiesTable betterTable(0, "table.name()", "table.profilepic()", 1, 1);
    std::vector<char> data = betterTable.serialize();
    std::vector<char> msg = packet.construct(std::string(data.begin(), data.end()));
    bufferevent_write(bufferEvent, &msg[0], msg.size());
}

void Thread::removeClient(Client *client)
{
	auto it = std::find(clients.begin(), clients.end(), client);
	clients.erase(it);
	util::LibEventUtil::freeBufferEvent(client->bufferEvent);
	delete client;
}

void Thread::broadcast(const std::string &message, int channel)
{
	for (Client *client : clients) {
		if (client->channel == channel) {
			bufferevent_lock(client->bufferEvent);
			bufferevent_write(client->bufferEvent, message.c_str(), message.size());
			bufferevent_unlock(client->bufferEvent);
		}
	}
}

void Thread::readCallback(struct bufferevent *bufferEvent, void *data)
{
    std::vector<char> buffer;
    int n = evbuffer_get_length(bufferevent_get_input(bufferEvent));
    buffer.resize(n);
    bufferevent_read(bufferEvent, &buffer[0], n);
    int ret = network::PacketDecoder::receivePacket(buffer, (PacketData*) data);
    if (ret != 1)
        network::ErrorPacket::dispatch(bufferEvent, (Error) ret);
}

void Thread::eventCallback(struct bufferevent *bufferEvent, short events,
		void *data)
{
	if (events & BEV_EVENT_TIMEOUT) {
	    log::Logger::log(log::DEBUG, "A client has timed out, closing connection!");
		Client *client = (Client*) data;
		client->thread.removeClient(client);
	}
}

} /* namespace thread */
} /* namespace auth */
