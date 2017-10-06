#ifndef ACCORD_THREAD_THREAD_H
#define ACCORD_THREAD_THREAD_H

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/util.h>
#include <openssl/ssl.h>
#include <thread>
#include <string>
#include <vector>

#include <accordshared/network/PacketData.h>

namespace accord {

class Server;

namespace thread {
    
struct Client : public PacketData{
	Client(Server &server) : server(server) { };
	Server &server;
	struct bufferevent* bufferEvent;
	int channel;
};

class Thread {
public:
    Thread(Server &server);
    ~Thread();
    
    void stop();
    void wake();
    void start();
    void acceptClient(evutil_socket_t clientSocket, SSL *ssl);
    void broadcast(const std::string &message, int channel);

    //callbacks
    static void readCallback(struct bufferevent *bufferEvent, void *data);
    static void eventCallback(struct bufferevent *bufferEvent, short events,
    		void *data);
    
    struct event_base *eventBase;
private:
    Server &server;
    std::thread thread;
    std::vector<Client*> clients; //libevent can't foreach on bufferevents :((
    
    void run();
};

} /* namespace thread */
} /* namespace accord */

#endif
