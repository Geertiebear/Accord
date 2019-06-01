#ifndef ACCORD_PERMISSION_LOGIC_H
#define ACCORD_PERMISSION_LOGIC_H

#include <stdint.h>
#include <accordserver/thread/Thread.h>
#include <accordshared/types/Return.h>
#include <boost/optional.hpp>

namespace accord {

/*
 * Helper functions dealing with permissions/roles
 */

bool canReadChannel(uint64_t user, uint64_t channel, thread::Client *client);
boost::optional<types::RolesRet> getRolesForCommunity(uint64_t community,
                                     thread::Client *client);

} /* namespace accord */

#endif
