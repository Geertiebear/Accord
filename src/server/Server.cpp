#include <accordserver/Server.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <memory>

#include <accordserver/log/Logger.h>
#include <accordserver/Arguments.h>
#include <accordserver/database/Database.h>
#include <accordserver/util/OpenSSLUtil.h>
#include <accordserver/util/CryptoUtil.h>
#include <accordserver/network/PacketHandlers.h>
#include <accordshared/network/PacketDecoder.h>

namespace accord {
    
std::vector<network::ReceiveHandler> Server::handlers = {
	&network::PacketHandlers::receiveSendMessagePacket,
	&network::PacketHandlers::receiveErrorPacket,
	&network::PacketHandlers::receiveDisconnectPacket
};

Server::Server(Arguments args) : numThreads(args.threads), port(args.port)
{
    ConfigLoader configLoader(args.config);
    config = configLoader.load();
    ctx = util::OpenSSLUtil::getContext(config);
    network::PacketDecoder::init();
    network::PacketHandler::init(handlers);
    threads.reserve(numThreads);

    verifyDatabase(args);
    runTests();
    
    setupThreads();
    setupSocket();

    running = true;
    
    acceptClients();
}

Server::~Server()
{
    if (running) {
        log::Logger::log(log::WARNING, "server deconstructor was called while it was still running!");
    }
    closeSocket();
    SSL_CTX_free(ctx);
}

void Server::stop()
{
    running = false;
}

void Server::broadcast(const std::string &message, int channel)
{
	for (int i = 0; i < numThreads; i++)
	    threads.at(i)->broadcast(message, channel);
}

void Server::verifyDatabase(const Arguments &args)
{
    database::Database database(config.database);
    database.connect();
    if (!database.verify() && args.initdatabase) {
        log::Logger::log(log::INFO, "Initializing database!");
        database.initDatabase();
    } else if (!database.verify()) {
        log::Logger::log(log::ERROR, "Database is not complete!");
        throw std::runtime_error("");
    }
    database.disconnect();
}

void Server::runTests()
{
    log::Logger::log(log::INFO, "Running tests...");
    std::string random = util::CryptoUtil::getRandomString(32);
    if (random.empty())
        exit(1);
    log::Logger::log(log::INFO, "Passed all tests!");
}

void Server::setupThreads()
{
    log::Logger::log(log::INFO, "Initializing server with " + std::to_string(numThreads) + " threads!");
    for (int i = 0; i < numThreads; i++) {
        auto newThread = std::make_shared<thread::Thread>(*this, config);
        newThread->start();
        threads.push_back(newThread);
    }
}

void Server::setupSocket() 
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        log::Logger::log(log::ERROR, "Error opening server socket!");
        throw std::runtime_error("");
    }
    
    serverAddr = { }; // zero out the struct before use
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(serverSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0 ) {
        log::Logger::log(log::ERROR, "Error binding to socket!");
        throw std::runtime_error("");
    }
    
    listen(serverSocket, 5);
    log::Logger::log(log::INFO, "Opened socket on " + std::to_string(port) + "!");
}

void Server::closeSocket()
{
    close(serverSocket);
}

void Server::acceptClients()
{
    while(running) {
        log::Logger::log(log::INFO, "Listening for clients!");
        
        SSL *ssl;

        evutil_socket_t clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket < 0) {
            log::Logger::log(log::ERROR, "Error on accept!");
            throw std::runtime_error("");
        }
        
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, clientSocket);
        SSL_accept(ssl);

        log::Logger::log(log::INFO, "Got client, pushing it to thread now!");
		
        int threadNum = selectThread();
	thread::Thread *thread = threads.at(threadNum).get();
	thread->acceptClient(clientSocket, ssl);
    }
}

int Server::selectThread()
{
    //round robin approach
    if (lastThread == numThreads)
    	lastThread = 0;
    return lastThread++;
}

} /* namespace accord */
