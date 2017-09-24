#ifndef ACCORD_SERVER_H
#define ACCORD_SERVER_H

#include <netinet/in.h>
#include <string>
#include <vector>
#include <memory>

#include <accordserver/thread/Thread.h>
#include <accordshared/network/PacketHandler.h>

namespace accord {

struct Arguments;

class Server {
public:
    Server(Arguments args);
    ~Server();
    
    void stop();
    void broadcast(const std::string &buffer);
private:
    struct sockaddr_in serverAddr;
    int numThreads;
    int port;
    int serverSocket;
	int lastThread = 0;
    bool running;
    
    void setupThreads();
    void setupSocket();
	void closeSocket();
    void acceptClients();
    int selectThread();
    
    std::vector<std::shared_ptr<thread::Thread>> threads;

	static std::vector<network::ReceiveHandler> handlers;
};

} /* namespace auth */

#endif
