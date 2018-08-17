#include <accordserver/network/PacketHandlers.h>

#include <accordserver/log/Logger.h>
#include <accordserver/Server.h>
#include <accordserver/Authentication.h>
#include <accordserver/thread/Thread.h>
#include <accordserver/util/CryptoUtil.h>

#include <accordshared/network/packet/RequestDataPacket.h>
#include <accordshared/network/packet/SerializationPacket.h>
#include <accordshared/network/packet/TokenPacket.h>
#include <accordshared/network/packet/ErrorPacket.h>
#include <accordshared/error/ErrorCodes.h>
#include <accordshared/types/Request.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <iostream>

namespace accord {
namespace network {

util::FunctionMap PacketHandlers::serializationMap = {
    { ADD_COMMUNITY_REQUEST, &PacketHandlers::handleAddCommunityRequest }
};

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
        const auto msg = packet.construct(ARGS_ERR);
        client->write(msg);
        return false;
    }
    std::string token = Authentication::authUser(client->thread.database,
                                                 strings[0], strings[1]);
    //authenticate the user
    client->token = token;
    if (token.length() != TOKEN_LEN) {
        network::ErrorPacket packet;
        const auto msg = packet.construct(AUTH_ERR);
        client->write(msg);
        return false;
    }
    client->user = client->thread.database.getUser(strings[0]);
    client->authenticated = true;
    log::Logger::log(log::DEBUG, "Successfully authenticated client!");

    //send token to client
    network::TokenPacket packet;
    std::vector<char> message = packet.construct(token);
    client->write(message);

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
        const auto msg = packet.construct(REGIST_ERR);
        client->write(msg);
        return false;
    }

    std::string token = Authentication::authUser(client->thread.database,
                                                 strings[0], strings[2]);
    network::TokenPacket packet;
    std::vector<char> message = packet.construct(token);
    client->write(message);

    return true;
}

bool PacketHandlers::receiveNoopPacket(const std::vector<char> &body, PacketData *data)
{
    thread::Client *client = (thread::Client*) data;
    log::Logger::log(log::WARNING, "Received packet NOOP from " + client->user.name());
    return true;
}

bool PacketHandlers::receiveRequestDataPacket(const std::vector<char> &body, PacketData *data)
{
    thread::Client *client = (thread::Client*) data;
    uint16_t requestId = util::BinUtil::assembleUint16((uint8_t) body[0], (uint8_t) body[1]);
    switch (requestId) {
        case network::COMMUNITIES_TABLE_REQUEST: {
            if (!client->authenticated) {
                network::ErrorPacket packet;
                const auto msg = packet.construct(AUTH_ERR);
                client->write(msg);
                break;
            }
            auto communities = client->thread.database.getCommunitiesForUser(client->user.id());
            std::vector<types::CommunitiesTable> shared;
            for (database::table_communities community : communities)
                shared.push_back(database::Database::communityServerToShared(community));

            network::SerializationPacket packet;
            for (types::CommunitiesTable table : shared) {
                auto data = util::Serialization::serialize(table);
                auto msg = packet.construct(network::COMMUNITIES_TABLE_REQUEST, std::string(data.begin(), data.end()));
                client->write(msg);
            }
            break;
        }
    }
    return true;
}

bool PacketHandlers::receiveSerializationPacket(const std::vector<char> &body, PacketData *data)
{
    return util::Serialization::receive(serializationMap, body, data);
}

bool PacketHandlers::handleAddCommunityRequest(PacketData *data, const std::vector<char> &body)
{
    types::AddCommunity request = util::Serialization::deserealize<
            types::AddCommunity>(body);

    thread::Client *client = (thread::Client*) data;
    uint64_t communityId = util::CryptoUtil::getRandomUINT64();
    database::table_communities community;
    if (!client->thread.database.initCommunity(communityId,
                                               client->user.id(),
                                               request, &community)) {
        return false; //TODO: send error packet
    }

    //we added a community, now let's send it back so that they can add it to their list
    types::CommunitiesTable table = database::Database::communityServerToShared(community);
    network::SerializationPacket packet;
    auto json = util::Serialization::serialize(table);
    auto msg = packet.construct(network::COMMUNITIES_TABLE_REQUEST, std::string(json.begin(), json.end()));
    client->write(msg);

    //done for now
    return true;
}

} /* namespace network */
} /* namespace accord */
