#include <Server.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <memory>

#include <log/Logger.h>
#include <Arguments.h>

namespace accord {
    
Server::Server(Arguments args) : numThreads(args.threads), port(args.port)
{
    queues.reserve(numThreads);
    //threads.reserve(numThreads);
    
    setupQueues();
    setupThreads();
    setupSocket();

    running = true;
    
    acceptClients();
}

Server::~Server()
{
    if (running) {
        Logger::log(WARNING, "server deconstructor was called while it was still running!");
    }
}

void Server::stop()
{
    running = false;
}

void Server::setupQueues()
{
    for (int i = 0; i < numThreads; i++) {
        std::unique_ptr<thread::WorkQueue> newQueue = std::make_unique<thread::WorkQueue>();
        queues.push_back(std::move(newQueue));
    }
}

void Server::setupThreads()
{
    for (int i = 0; i < numThreads; i++) {
        thread::WorkQueue *queue = queues.at(i).get();
        std::shared_ptr<thread::Thread> newThread = std::make_shared<thread::Thread>(queue);
        newThread->start();
        threads.push_back(newThread);
    }
}

void Server::setupSocket() 
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        Logger::log(ERROR, "Error opening server socket!");
        throw std::runtime_error("");
    }
    
    serverAddr = { }; // zero out the struct before use
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(serverSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0 ) {
        Logger::log(ERROR, "Error binding to socket!");
        throw std::runtime_error("");
    }
    
    listen(serverSocket, 5);
    Logger::log(INFO, "Opened socket on " + std::to_string(port) + "!");
}

void Server::acceptClients()
{
    while(running) {
        thread::Work newWork = {};

        Logger::log(INFO, "Listening for clients!");
        newWork.clientSocket = accept(serverSocket, NULL, NULL);
        if (newWork.clientSocket < 0) {
            Logger::log(ERROR, "Error on accept!");
            throw std::runtime_error("");
        }
    
        Logger::log(INFO, "Got client, pushing it to thread's work queue now!");
        
        int thread = selectThread();
        queues.at(thread).get()->push_back(newWork);
        if (!threads.at(thread).get()->isAwake())
            threads.at(thread)->wake();
    }
}

int Server::selectThread()
{
    int lowestNum = queues.at(0).get()->size();
    int lowest = 0;
    
    for (int i = 1; i < numThreads; i++) {
        thread::WorkQueue *queue = queues.at(i).get();
        int queueSize = queue->size();
        if (queueSize < lowestNum) {
            lowestNum = queueSize;
            lowest = i;
        }
    }
    
    Logger::log(DEBUG, std::to_string(lowestNum));
    return lowest;
}

} /* namespace accord */
