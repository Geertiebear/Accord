#include <accordserver/thread/Thread.h>

#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <system_error>

#include <accordserver/Server.h>
#include <accordserver/log/Logger.h>
#include <accordshared/network/PacketDecoder.h>

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

void Thread::acceptClient(evutil_socket_t clientSocket)
{
	struct bufferevent *bufferEvent = bufferevent_socket_new(eventBase,
			clientSocket, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
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
	//for now we copy the client's shit into a buffer and print it out 1024 bytes at a time
	char packetIdBuffer[2];
	size_t n;
	memset(&packetIdBuffer, '\0', sizeof(packetIdBuffer));
	n = bufferevent_read(bufferEvent, packetIdBuffer, sizeof(packetIdBuffer));

	std::string packetIdString(packetIdBuffer);
	network::PacketId packetId = std::stoi(packetIdString);
	Logger::log(DEBUG, "Received packet id is: " + std::to_string(packetId));

	const network::Packet &packet = network::PacketDecoder::getPacket(packetId);
	char bodyBuffer[packet.getBufferSize()];
	memset(&bodyBuffer, '\0', sizeof(bodyBuffer));
	n = bufferevent_read(bufferEvent, bodyBuffer, sizeof(bodyBuffer));
	std::vector<std::string> args;
	args.push_back(std::string(bodyBuffer));
	packet.receive(args);
}

void Thread::eventCallback(struct bufferevent *bufferEvent, short events,
		void *data)
{
	if (events & BEV_EVENT_TIMEOUT) {
		Logger::log(DEBUG, "A client has timed out, closing connection!");
		bufferevent_free(bufferEvent);
	}
}

void Thread::work()
{
    pid_t tid = syscall(SYS_gettid);
    Logger::log(DEBUG, "Work function called from thread tid " + std::to_string(tid));
}

} /* namespace thread */
} /* namespace auth */