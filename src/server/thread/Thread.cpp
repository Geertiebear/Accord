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
#include <accordserver/log/Logger.h>
#include <accordserver/util/LibEventUtil.h>
#include <accordshared/error/ErrorCodes.h>
#include <accordshared/network/PacketDecoder.h>
#include <accordshared/network/packet/ErrorPacket.h>
#include <accordshared/util/BinUtil.h>

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
	char *packetIdBuffer = (char*) malloc(2);
	size_t n;
	memset(packetIdBuffer, '\0', 2);
	bufferevent_read(bufferEvent, packetIdBuffer, 2);
	
	uint8_t low = (uint8_t) packetIdBuffer[0];
	uint8_t high = (uint8_t) packetIdBuffer[1];

	network::PacketId packetId = util::BinUtil::assembleUint16(low, high);
	log::Logger::log(log::DEBUG, "Received packet id is: " + std::to_string(packetId));
	free(packetIdBuffer);

	const network::Packet *packet = network::PacketDecoder::getPacket(packetId);
	if (!packet) {
		network::ErrorPacket::dispatch(bufferEvent, Error::NOT_FOUND);
		return;
	}
	const int bufferSize = packet->getBufferSize();
	char *bodyBuffer = (char*) malloc(bufferSize);
	memset(bodyBuffer, '\0', bufferSize);
	struct evbuffer *readBuffer = bufferevent_get_input(bufferEvent);
	size_t size = evbuffer_get_length(readBuffer);
	if (size > packet->getBufferSize()) {
		network::ErrorPacket::dispatch(bufferEvent, Error::TOO_LONG);
		log::Logger::log(log::ERROR, "Client sent message too long!" + std::to_string(size));
		return;
	}
	n = bufferevent_read(bufferEvent, bodyBuffer, bufferSize);
	Client *client = reinterpret_cast<Client*>(data);
	packet->receive(std::string(bodyBuffer), client);
	free(bodyBuffer);
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
