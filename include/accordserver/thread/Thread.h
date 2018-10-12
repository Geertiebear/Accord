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
#include <deque>

#include <accordserver/database/Database.h>
#include <accordserver/Config.h>
#include <accordshared/network/PacketDecoder.h>
#include <accordshared/network/PacketData.h>
#include <accordshared/error/ErrorCodes.h>

namespace accord {

class Server;

namespace thread {
    
class Thread;

/*
 * struct for holding info about partially read packets
 */
struct PacketBuffer {
    network::PacketId id;
    uint64_t length;
    std::vector<char> buffer;
};

struct Client : public PacketData {
    Client(Server &server, Thread &thread) : server(server), thread(thread) { }
	Server &server;
	Thread &thread;
	struct bufferevent* bufferEvent;
	int channel;
    database::table_users user;
    bool hasPartialPacket;
    PacketBuffer partialPacket;
    std::deque<char> writeBuffer;

    void write(const std::vector<char> &msg);
};

class Thread {
public:
    Thread(Server &server, const Config &config);
    ~Thread();
    
    void stop();
    void wake();
    void start();
    void disconnectClient(Client *client);
    void removeClient(Client *client);
    void acceptClient(evutil_socket_t clientSocket, SSL *ssl);
    void broadcast(const std::vector<char> &data);
    /* -1 if expired, 0 if invalid, 1 if valid */
    int isInviteValid(const std::string &invite);
    uint64_t getCommunityForInvite(const std::string &invite);
    std::string genInvite(uint64_t communityId);

    //callbacks
    static void readCallback(struct bufferevent *bufferEvent, void *data);
    static void writeCallback(struct bufferevent *bufferEvent, void *data);
    static void eventCallback(struct bufferevent *bufferEvent, short events,
    		void *data);
    
    struct event_base *eventBase;
    database::Database database;
private:
    Server &server;
    const Config &config;
    std::thread thread;
    std::vector<Client*> clients; //libevent can't foreach on bufferevents :((

    void run();
    int checkInviteInDatabase(const std::string &invite);
    static void handlePartialPacket(struct bufferevent *bufferEvent, Client *client);
    static void dispatchError(Client *client, Error error);
};

} /* namespace thread */
} /* namespace accord */

#endif
