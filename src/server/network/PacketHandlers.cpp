#include <accordserver/network/PacketHandlers.h>

#include <accordserver/log/Logger.h>
#include <accordserver/Server.h>
#include <accordserver/Authentication.h>
#include <accordserver/thread/Thread.h>

#include <accordshared/network/packet/TokenPacket.h>
#include <accordshared/network/packet/ErrorPacket.h>
#include <accordshared/error/ErrorCodes.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <iostream>

namespace accord {
namespace network {

bool PacketHandlers::receiveSendMessagePacket(const std::vector<char> &body, PacketData *data)
{
	thread::Client *client = (thread::Client*) data;
    client->server.broadcast(std::string(body.begin(), body.end()), client->channel);
    log::Logger::log(log::DEBUG, "Received send message packet with body: " +
                     std::string(body.begin(), body.end()));
	return true;
}

bool PacketHandlers::receiveErrorPacket(const std::vector<char> &body, PacketData *data)
{
    log::Logger::log(log::DEBUG, "Received error packet with body: " +
                     std::string(body.begin(), body.end()));
	return true;
}

bool PacketHandlers::receiveDisconnectPacket(const std::vector<char> &body, PacketData *data)
{
	thread::Client *client = (thread::Client*) data;
	client->thread.removeClient(client);
	log::Logger::log(log::DEBUG, "Removed client");
	return true;
}

bool PacketHandlers::receiveAuthPacket(const std::vector<char> &body, PacketData *data)
{
    //get arguments
    thread::Client *client = (thread::Client*) data;
    std::string bodyString(body.begin(), body.end());
    std::vector<std::string> strings;
    boost::split(strings, bodyString, boost::is_any_of(
                 std::string(1, (char)0x3)),
                 boost::token_compress_on);
    if (strings.size() != 2) {
        network::ErrorPacket packet;
        packet.dispatch(client->bufferEvent, ARGS_ERR);
        return false;
    }
    std::string token = Authentication::authUser(client->thread.database,
                                                 strings[0], strings[1]);
    //authenticate the user
    client->token = token;
    if (token.length() != TOKEN_LEN) {
        network::ErrorPacket packet;
        packet.dispatch(client->bufferEvent, AUTH_ERR);
        return false;
    }
    client->user = client->thread.database.getUser(strings[0]);
    log::Logger::log(log::DEBUG, "Successfully authenticated client!");

    //send token to client
    network::TokenPacket packet;
    std::vector<char> message = packet.construct(token);
    bufferevent_write(client->bufferEvent, &message[0], message.size());

    return true;
}

bool PacketHandlers::receiveRegisterPacket(const std::vector<char> &body, PacketData *data)
{
    thread::Client *client = (thread::Client*) data;
    std::string bodyString(body.begin(), body.end());
    std::vector<std::string> strings;
    boost::split(strings, bodyString, boost::is_any_of(
                 std::string(1, (char)0x3)),
                 boost::token_compress_on);
     if (strings.size() != 3)
        return false;
    bool ret = Authentication::registerUser(client->thread.database, strings[0],
                                 strings[1], strings[2]);
    if (!ret) {
        network::ErrorPacket packet;
        packet.dispatch(client->bufferEvent, REGIST_ERR);
        return false;
    }

    std::string token = Authentication::authUser(client->thread.database,
                                                 strings[0], strings[2]);
    network::TokenPacket packet;
    std::vector<char> message = packet.construct(token);
    bufferevent_write(client->bufferEvent, &message[0], message.size());

    return true;
}


} /* namespace network */
} /* namespace accord */
