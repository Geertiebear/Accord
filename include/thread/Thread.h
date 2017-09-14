#ifndef ACCORD_THREAD_THREAD_H
#define ACCORD_THREAD_THREAD_H

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/util.h>
#include <thread>

namespace accord {
namespace thread {
    
class Thread {
public:
    Thread();
    ~Thread();
    
    void stop();
    void wake();
    void start();
	void acceptClient(evutil_socket_t clientSocket);

	//callbacks
	static void readCallback(struct bufferevent *bufferEvent, void *data);
	static void eventCallback(struct bufferevent *bufferEvent, short events,
			void *data);
    
	struct event_base *eventBase;
private:
    std::thread thread;
    
    void work();
    void run();
};

} /* namespace thread */
} /* namespace accord */

#endif
