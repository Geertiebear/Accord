#ifndef ACCORD_TYPES_PERMISSIONS_H
#define ACCORD_TYPES_PERMISSIONS_H

#include <stdint.h>

namespace accord {
namespace types {

enum CommunityPermissions : uint64_t {
    READ_CHANNELS = 0,
    SEND_MESSAGES
};

enum ChannelPermissions : uint64_t {
    CHANNEL_READ = 0,
    CHANNEL_SEND
};

} /* namespace types */
} /* namespace accord */

#endif
