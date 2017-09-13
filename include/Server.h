#ifndef ACCORD_SERVER_H
#define ACCORD_SERVER_H

#include <netinet/in.h>
#include <vector>
#include <memory>

#include <thread/Thread.h>

namespace accord {

struct Arguments;
    
class Server {
public:
    Server(Arguments args);
    ~Server();
    
    void stop();
    
private:
    struct sockaddr_in serverAddr;
    int numThreads;
    int port;
    int serverSocket;
    bool running;
    
    void setupQueues();
    void setupThreads();
    void setupSocket();
    void acceptClients();
    int selectThread();
    
    std::vector<std::unique_ptr<thread::WorkQueue>> queues;
    std::vector<std::shared_ptr<thread::Thread>> threads;
};

} /* namespace auth */

#endif
