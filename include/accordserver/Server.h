#ifndef ACCORD_SERVER_H
#define ACCORD_SERVER_H

#include <accordserver/util/Network.h>
#include <openssl/ssl.h>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <mutex>

#include <accordserver/thread/Thread.h>
#include <accordserver/Config.h>
#include <accordshared/network/PacketHandler.h>
#include <accordshared/util/Serialization.h>
#include <accordshared/types/Return.h>

namespace accord {

struct Arguments;

struct OnlineUser {
    OnlineUser(int refCount) : refCount(refCount)
    {}
    int refCount = 0;
    std::vector<thread::Client*> clients;
};

class Server {
public:
    Server(Arguments args);
    ~Server();
    
    void stop();
    void broadcast(const std::vector<char> &data);
    bool isInviteValid(const std::string &invite);
    uint64_t getCommunityForInvite(const std::string &invite);
    void insertInvite(uint64_t communityId, const std::string &invite);

    std::list<types::UserData> getOnlineList(uint64_t channelId,
                                             thread::Client *client);
    void registerOnlineUser(const types::UserData &user,
                              thread::Client *client);
    void removeOnlineUser(uint64_t user, thread::Client *client);
    void notifyStatusChange(uint64_t user, thread::Client *client);
private:
    ACCORD_SOCKADDR serverAddr;
    int numThreads;
    int port;
    int serverSocket;
    int lastThread = 0;
    bool running;
    SSL_CTX *ctx;
    Config config;

    void verifyDatabase(const Arguments &args);
    void runTests();
    void setupThreads();
    void setupSocket();
    void closeSocket();
    void acceptClients();
    int selectThread();
    
    std::vector<std::shared_ptr<thread::Thread>> threads;
    std::map<std::string, uint64_t> inviteMap;
    std::mutex inviteMapMutex;
    std::map<uint64_t, OnlineUser> onlineMap;
    std::mutex onlineMapMutex;

    static std::vector<network::ReceiveHandler> handlers;
};

} /* namespace auth */

#endif
