#include <accordserver/PermissionLogic.h>
#include <accordshared/types/Permissions.h>

namespace accord {

bool canReadChannel(uint64_t user, uint64_t channel, thread::Client *client)
{
    if (!client->thread.database.isUserInChannel(user, channel))
        return false;
    auto community = client->thread.database.getCommunityForChannel(channel);
    if (!client->thread.database.hasCommunityPermission(user,
                                                        community.get().id,
                                                        types::READ_CHANNELS))
        return false;
    if (!client->thread.database.hasChannelPermission(user, channel,
                                                      types::CHANNEL_READ))
        return false;
    return true;
}

boost::optional<types::RolesRet> getRolesForCommunity(uint64_t community,
                                                      thread::Client *client)
{
    const auto roles = client->thread.database.getRolesForCommunity(community);
    std::list<types::Role> list;
    for (const auto role : roles) {
        const auto communityPermissions = client->thread.database.
                getCommunityPermissions(role.id);
        const auto channelPermissions = client->thread.database.
                getChannelPermissions(role.id);
        list.emplace_back(role.id, community, role.name, role.colour,
                          communityPermissions, channelPermissions);
    }
    return types::RolesRet(list, community);
}

} /* namespace accord */
