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

namespace accord {
namespace thread {

Thread::Thread(Server &server) : server(server), thread()
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
    thread = std::thread(&Thread::run, this);
    try {
        thread.detach();
    } catch (std::system_error &e) {
        Logger::log(ERROR, "Error detatching thread!");
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
	bufferevent_setcb(bufferEvent, &Thread::readCallback, NULL,
			&Thread::eventCallback, this);
	bufferevent_set_timeouts(bufferEvent, &readTimeout, NULL);
	bufferevent_enable(bufferEvent, EV_READ | EV_WRITE);
	bufferEvents.push_back(bufferEvent);
}

void Thread::broadcast(const std::string &message)
{
	for (bufferevent *bufferEvent : bufferEvents) {
		bufferevent_lock(bufferEvent);
		bufferevent_write(bufferEvent, message.c_str(), message.size());
		bufferevent_unlock(bufferEvent);
	}
}

void Thread::readCallback(struct bufferevent *bufferEvent, void *data)
{
	char packetIdBuffer[2];
	size_t n;
	memset(&packetIdBuffer, '\0', sizeof(packetIdBuffer));
	bufferevent_read(bufferEvent, packetIdBuffer, sizeof(packetIdBuffer));

	network::PacketId packetId;
	std::string packetIdString(packetIdBuffer);
	try {
		packetId = std::stoi(packetIdString);
	} catch (std::invalid_argument &e) {
		network::ErrorPacket::dispatch(bufferEvent, Error::NAN);
		return;
	}
	Logger::log(DEBUG, "Received packet id is: " + std::to_string(packetId));

	const network::Packet *packet = network::PacketDecoder::getPacket(packetId);
	if (!packet) {
		network::ErrorPacket::dispatch(bufferEvent, Error::NOT_FOUND);
		return;
	}
	char bodyBuffer[packet->getBufferSize()];
	memset(&bodyBuffer, '\0', sizeof(bodyBuffer));
	struct evbuffer *readBuffer = bufferevent_get_input(bufferEvent);
	size_t size = evbuffer_get_length(readBuffer);
	if (size > packet->getBufferSize()) {
		network::ErrorPacket::dispatch(bufferEvent, Error::TOO_LONG);
		Logger::log(ERROR, "Client sent message too long!" + std::to_string(size));
		return;
	}
	n = bufferevent_read(bufferEvent, bodyBuffer, sizeof(bodyBuffer));
	packet->receive(std::string(bodyBuffer));
}

void Thread::eventCallback(struct bufferevent *bufferEvent, short events,
		void *data)
{
	if (events & BEV_EVENT_TIMEOUT) {
		Logger::log(DEBUG, "A client has timed out, closing connection!");
		util::LibEventUtil::freeBufferEvent(bufferEvent);
	}
}

} /* namespace thread */
} /* namespace auth */
