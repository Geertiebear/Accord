#ifndef ACCORD_THREAD_THREAD_H
#define ACCORD_THREAD_THREAD_H

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/util.h>
#include <thread>
#include <string>
#include <vector>

namespace accord {

class Server;

namespace thread {
    
class Thread {
public:
    Thread(Server &server);
    ~Thread();
    
    void stop();
    void wake();
    void start();
	void acceptClient(evutil_socket_t clientSocket);
	void broadcast(const std::string &message);

	//callbacks
	static void readCallback(struct bufferevent *bufferEvent, void *data);
	static void eventCallback(struct bufferevent *bufferEvent, short events,
			void *data);
    
	struct event_base *eventBase;
private:
	Server &server;
    std::thread thread;
	std::vector<bufferevent*> bufferEvents; //libevent can't foreach on bufferevents :((
    
    void work();
    void run();
};

} /* namespace thread */
} /* namespace accord */

#endif
