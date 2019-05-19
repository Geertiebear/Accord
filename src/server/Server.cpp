#include <accordserver/Server.h>

#include <sys/types.h>
#include <memory>

#include <accordserver/log/Logger.h>
#include <accordserver/Arguments.h>
#include <accordserver/database/Database.h>
#include <accordserver/util/OpenSSLUtil.h>
#include <accordserver/util/CryptoUtil.h>
#include <accordserver/network/PacketHandlers.h>
#include <accordshared/network/PacketDecoder.h>
#include <accordshared/network/packet/SerializationPacket.h>

namespace accord {
    
std::vector<network::ReceiveHandler> Server::handlers = {
	&network::PacketHandlers::receiveSendMessagePacket,
	&network::PacketHandlers::receiveErrorPacket,
    &network::PacketHandlers::receiveDisconnectPacket,
    &network::PacketHandlers::receiveAuthPacket,
    &network::PacketHandlers::receiveRegisterPacket,
    &network::PacketHandlers::receiveNoopPacket,
    &network::PacketHandlers::receiveSerializationPacket,
    &network::PacketHandlers::receiveKeepAlivePacket,
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

void Server::broadcast(const std::vector<char> &data)
{
	for (int i = 0; i < numThreads; i++)
        threads.at(i)->broadcast(data);
}

bool Server::isInviteValid(const std::string &invite)
{
    std::unique_lock<std::mutex> lock(inviteMapMutex);
    auto it = inviteMap.find(invite);
    return it != inviteMap.end();
}

uint64_t Server::getCommunityForInvite(const std::string &invite)
{
    std::unique_lock<std::mutex> lock(inviteMapMutex);
    return inviteMap[invite];
}

void Server::insertInvite(uint64_t communityId, const std::string &invite)
{
    std::unique_lock<std::mutex> lock(inviteMapMutex);
    inviteMap.insert(std::make_pair(invite, communityId));
}

std::list<types::UserData> Server::getOnlineList(uint64_t channelId,
                                                 thread::Client *client)
{
    /* TODO: maybe in the future we should cache this */
    const auto members = client->thread.database.getUsersForChannel(channelId);
    std::list<types::UserData> ret;
    std::unique_lock<std::mutex> lock(onlineMapMutex);
    for (const auto user : members) {
        if (onlineMap.find(user.id) != onlineMap.end()) {
            types::UserData smallUser(user.id, user.name, user.profilepic);
            ret.push_back(smallUser);
        }
    }
    return ret;
}

void Server::registerOnlineUser(const types::UserData &user,
                                  thread::Client *client)
{
    std::unique_lock<std::mutex> lock(onlineMapMutex);
    auto it = onlineMap.find(user.id);
    if (it == onlineMap.end()) {
        OnlineUser onlineUser(1);
        onlineUser.clients.push_back(client);
        onlineMap.insert(std::make_pair(user.id, onlineUser));
    } else {
        OnlineUser &onlineUser = onlineMap.at(user.id);
        onlineUser.refCount++;
        onlineUser.clients.push_back(client);
    }
}

void Server::removeOnlineUser(uint64_t user, thread::Client *client)
{
    std::unique_lock<std::mutex> lock(onlineMapMutex);
    auto it = onlineMap.find(user);
    if (it == onlineMap.end())
        return; /* TODO: maybe report an error if debugging? */
    OnlineUser &onlineUser = onlineMap.at(user);
    onlineUser.refCount--;
    if (onlineUser.refCount <= 0) {
        onlineMap.erase(it);
        return;
    }
    auto &clients = onlineUser.clients;
    clients.erase(std::remove(clients.begin(), clients.end(), client),
                  clients.end());
}

void Server::notifyStatusChange(uint64_t user, thread::Client *client)
{
    const auto channels = client->thread.database.getChannelsForUser(user);
    for (auto channel : channels) {
        const auto members = client->thread.database.getUsersForChannel(
                    channel.id);
        std::unique_lock<std::mutex> lock(onlineMapMutex);
        for (auto member : members) {
            if (member.id == user)
                continue;

            auto it = onlineMap.find(member.id);
            if (it == onlineMap.end())
                continue;
            const auto onlineUser = onlineMap.at(user);

            const auto listToSend = getOnlineList(channel.id, client);
            types::OnlineListRet ret(listToSend, channel.id);
            network::SerializationPacket packet;
            const auto json = util::Serialization::serialize(ret);
            const auto msg = packet.construct(types::ONLINE_LIST_REQUEST, json);
            for (auto client : onlineUser.clients)
                client->write(msg);
        }
    }
}

void Server::verifyDatabase(const Arguments &args)
{
    database::Database database(config.database);
    database.connect();
    if (!database.verify() && args.initdatabase) {
        log::Logger::log(log::INFO, "Initializing database!");
        database.initDatabase();
    } else if (!database.verify()) {
        log::Logger::log(log::FATAL, "Database is not complete!");
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
        log::Logger::log(log::FATAL, "Error opening server socket!");
        throw std::runtime_error("");
    }
    
    serverAddr = { }; // zero out the struct before use
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(serverSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0 ) {
        log::Logger::log(log::FATAL, "Error binding to socket!");
        throw std::runtime_error("");
    }
    
    listen(serverSocket, 5);
    log::Logger::log(log::INFO, "Opened socket on " + std::to_string(port) + "!");
}

void Server::closeSocket()
{
    ACCORD_CLOSESOCKET(serverSocket);
}

void Server::acceptClients()
{
    while(running) {
        log::Logger::log(log::INFO, "Listening for clients!");
        
        SSL *ssl;

        evutil_socket_t clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket < 0) {
            log::Logger::log(log::FATAL, "Error on accept!");
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
