#include <accordserver/PermissionLogic.h>
#include <accordshared/types/Permissions.h>

namespace accord {

bool PermissionLogic::__canReadChannel(
            const std::vector<types::CommunityPermission> &communityPermissions,
            const std::vector<types::ChannelPermission> &channelPermissions)
{
    /* TODO: some permissions take precedence over others */
    for (const auto permission : channelPermissions) {
        if (permission.permission == types::CHANNEL_READ) {
            if (permission.allow == 1)
                return true;
            else if (permission.allow == -1)
                return false;
        }
    }

    /* the permission for the channel is defaulted, check community
     * permissions */
    for (const auto permission : communityPermissions)
        if (permission.permission == types::READ_CHANNELS)
            if (permission.allow == 1)
                return true;
    return false;
}

bool PermissionLogic::canReadChannel(uint64_t user,
                                     uint64_t channel, thread::Client *client)
{
    if (!client->thread.database.isUserInChannel(user, channel))
        return false;
    auto community = client->thread.database.getCommunityForChannel(channel);
    if (!community)
        return false;

    auto communityPermissions = client->thread.database.
            getCommunityPermissionsForUser(user, boost::get(community).id);
    if (communityPermissions.empty())
        return false;

    auto channelPermissions = client->thread.database.
            getChannelPermissionsForUser(user, channel);
    if (channelPermissions.empty())
        return false;
    return __canReadChannel(communityPermissions, channelPermissions);
}

boost::optional<types::RolesRet> PermissionLogic::getRolesForCommunity(
        uint64_t community, thread::Client *client)
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
