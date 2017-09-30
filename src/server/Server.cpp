#include <accordserver/Server.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <memory>

#include <accordserver/log/Logger.h>
#include <accordserver/Arguments.h>
#include <accordserver/util/OpenSSLUtil.h>
#include <accordserver/network/PacketHandlers.h>
#include <accordshared/network/PacketDecoder.h>

namespace accord {
    
std::vector<network::ReceiveHandler> Server::handlers = {
	&network::PacketHandlers::receiveSendMessagePacket,
	&network::PacketHandlers::receiveErrorPacket
};

Server::Server(Arguments args) : numThreads(args.threads), port(args.port),
    ctx(util::OpenSSLUtil::getContext())
{
    network::PacketDecoder::init();
    network::PacketHandler::init(handlers);
    threads.reserve(numThreads);
    
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
    closeSocket();
    SSL_CTX_free(ctx);
}

void Server::stop()
{
    running = false;
}

void Server::broadcast(const std::string &message)
{
	for (int i = 0; i < numThreads; i++)
	    threads.at(i)->broadcast(message);
}

void Server::setupThreads()
{
    for (int i = 0; i < numThreads; i++) {
        auto newThread = std::make_shared<thread::Thread>(*this);
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

void Server::closeSocket()
{
    close(serverSocket);
}

void Server::acceptClients()
{

    while(running) {
        Logger::log(INFO, "Listening for clients!");
        
        SSL *ssl;

        evutil_socket_t clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket < 0) {
            Logger::log(ERROR, "Error on accept!");
            throw std::runtime_error("");
        }
        
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, clientSocket);
        SSL_accept(ssl);

        Logger::log(INFO, "Got client, pushing it to thread now!");
		
        int threadNum = selectThread();
	thread::Thread *thread = threads.at(threadNum).get();
	thread->acceptClient(clientSocket, ssl);
    }
}

int Server::selectThread()
{
    //round robin approach
    if (lastThread == 4)
    	lastThread = 0;
    return lastThread++;
}

} /* namespace accord */
