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
    auto it = inviteMap.find(invite);
    return it != inviteMap.end();
}

uint64_t Server::getCommunityForInvite(const std::string &invite)
{
    return inviteMap[invite];
}

void Server::insertInvite(uint64_t communityId, const std::string &invite)
{
    inviteMap.insert(std::make_pair(invite, communityId));
}

std::list<types::UserData> Server::getOnlineList(uint64_t channelId)
{
    auto &list = onlineMap[channelId];
    std::list<types::UserData> ret;
    for (OnlineUser user : list)
        ret.push_back(user.user);
    return ret;
}

void Server::registerOnlineMember(uint64_t channel, const types::UserData &user,
                                  thread::Client *client)
{
    auto list = onlineMap[channel];
    auto it = std::find_if(list.begin(), list.end(), [&] (const OnlineUser &s) {
        return s.user == user;
    });
    if (it == list.end()) {
        OnlineUser onlineUser(1, user);
        onlineUser.clients.push_back(client);
        list.push_back(onlineUser);
    } else {
        OnlineUser onlineUser = *it;
        onlineUser.refCount++;
        onlineUser.clients.push_back(client);
        *it = onlineUser;
    }
    onlineMap[channel] = list;
}

void Server::removeOnlineMember(uint64_t channel, uint64_t user)
{
    auto list = onlineMap[channel];
    auto it = std::find_if(list.begin(), list.end(), [&] (const OnlineUser &s) {
        return s.user.id == user;
    });
    if (it == list.end())
        return;
    OnlineUser onlineUser = *it;
    onlineUser.refCount--;
    if (onlineUser.refCount <= 0) {
        list.erase(it);
        return;
    }
    *it = onlineUser;
}

void Server::notifyOnline(uint64_t id, thread::Client *client)
{
    const auto channels = client->thread.database.getChannelsForUser(id);
    for (database::table_channels channel : channels) {
        const auto &list = onlineMap.at(channel.id());
        for (OnlineUser user : list) {
            if (user.user.id == id)
                continue;

            const auto listToSend = getOnlineList(channel.id());
            types::OnlineListRet ret(listToSend, channel.id());
            network::SerializationPacket packet;
            const auto json = util::Serialization::serialize(ret);
            const auto msg = packet.construct(types::ONLINE_LIST_REQUEST, json);
            for (auto client : user.clients)
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
