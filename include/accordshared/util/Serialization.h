#ifndef ACCORD_UTIL_SERIALIZATION_H
#define ACCORD_UTIL_SERIALIZATION_H

#include <accordshared/util/Factory.h>
#include <accordshared/network/PacketData.h>
#include <map>
#include <string>
#include <functional>
#include <type_traits>
#include <boost/any.hpp>
#include <vector>

namespace accord {
namespace util {

using FunctionMap = std::map<std::string, std::function<bool(PacketData*, boost::any)>>;

class Serializable {
public:
    virtual std::vector<char> serialize() = 0;
    virtual boost::any deserialize(const std::vector<char> &data) = 0;
    static void initTypes(FunctionMap);
    static bool receive(const std::vector<char> &body, PacketData *data);
    static FunctionMap map;
    static Factory<Serializable> serializableFactory;

    //cant use c++17 because of mysql++ q.q
    //otherwise I would be using if constexpr for this
    template<typename T>
    static std::enable_if_t<std::is_same<T, std::string>::value> insertData(std::vector<char> &data, T toBeInsterted)
    {
        std::copy(toBeInsterted.begin(), toBeInsterted.end(), std::back_inserter(data));
    }

    template<typename T>
    static std::enable_if_t<std::is_same<T, int>::value> insertData(std::vector<char> &data, T toBeInsterted)
    {
        std::string string = std::to_string(toBeInsterted);
        std::copy(string.begin(), string.end(), std::back_inserter(data));
    }

    template<typename T>
    static std::enable_if_t<std::is_same<T, uint64_t>::value> insertData(std::vector<char> &data, T toBeInsterted)
    {
        std::string string = std::to_string(toBeInsterted);
        std::copy(string.begin(), string.end(), std::back_inserter(data));
    }
};

} /* namespace util */
} /* namespace accord */

#endif
