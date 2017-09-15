#include <thread/Thread.h>

#include <ctype.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <system_error>

#include <log/Logger.h>

namespace accord {
namespace thread {

Thread::Thread() : thread()
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
			clientSocket, BEV_OPT_CLOSE_ON_FREE);
	struct timeval readTimeout = {
		.tv_sec = 5,
		.tv_usec = 0,
	};
	bufferevent_setcb(bufferEvent, &Thread::readCallback, NULL,
			&Thread::eventCallback, NULL);
	bufferevent_set_timeouts(bufferEvent, &readTimeout, NULL);
	bufferevent_enable(bufferEvent, EV_READ | EV_WRITE);
}

void Thread::readCallback(struct bufferevent *bufferEvent, void *data)
{
	//for now we copy the client's shit into a buffer and print it out 1024 bytes at a time
	char buffer[1024];
	size_t n;
	while (1) {
		n = bufferevent_read(bufferEvent, buffer, sizeof(buffer));
		if (n <= 0)
			break;
		Logger::log(DEBUG, "Client's message is: " + std::string(buffer));
	}
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
