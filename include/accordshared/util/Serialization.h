#ifndef ACCORD_UTIL_SERIALIZATION_H
#define ACCORD_UTIL_SERIALIZATION_H

#include <accordshared/network/PacketData.h>
#include <accordshared/util/BinUtil.h>
#include <functional>
#include <map>
#include <sstream>
#include <cereal/archives/json.hpp>

namespace accord {
namespace util {

using FunctionMap = std::map<int, std::function<bool(PacketData*, const std::vector<char>&)>>;

class Serialization {
public:
    template<class T>
    static std::vector<char> serialize(T object)
    {
        std::stringstream ss;
        {
            cereal::JSONOutputArchive archive(ss);
            archive(object);
        }
        std::vector<char> ret;
        const std::string &str = ss.str();
        ret.assign(str.begin(), str.end());
        return ret;
    }

    template<class T>
    static T deserealize(const std::vector<char> &data)
    {
        std::stringstream ss(std::string(data.begin(), data.end()));
        cereal::JSONInputArchive archive(ss);
        T object;
        archive(object);
        return object;
    }

    static bool receive(FunctionMap &map, const std::vector<char> &body, PacketData *data)
    {
        uint16_t id = util::BinUtil::assembleUint16(body[0], body[1]);
        std::vector<char> editedBody(body.begin() + 2, body.end());
        return map[id](data, editedBody);
    }
};

} /* namespace util */
} /* namespace accord */

#endif
