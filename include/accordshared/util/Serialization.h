#ifndef ACCORD_UTIL_SERIALIZATION_H
#define ACCORD_UTIL_SERIALIZATION_H

#include <accordshared/util/Factory.h>
#include <accordshared/network/PacketData.h>
#include <map>
#include <string>
#include <functional>
#include <boost/any.hpp>
#include <vector>

namespace accord {
namespace util {

using FunctionMap = std::map<std::string, std::function<bool(PacketData*)>>;

class Serializable {
public:
    virtual std::vector<char> serialize() = 0;
    virtual boost::any deserialize(const std::vector<char> &data) = 0;
    static void initTypes(FunctionMap);
    static FunctionMap map;
    static Factory<Serializable> serializableFactory;
};

} /* namespace util */
} /* namespace accord */

#endif
