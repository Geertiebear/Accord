#include <accordshared/util/Serialization.h>
#include <accordshared/types/Database.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace accord {
namespace util {

FunctionMap Serializable::map;
Factory<Serializable> Serializable::serializableFactory;

void Serializable::initTypes(FunctionMap map)
{
    Serializable::map = map;
    serializableFactory.regist("CommunitiesTable",
                               &types::CommunitiesTable::Creator);
}

bool Serializable::receive(const std::vector<char> &body, PacketData *data)
{
    std::vector<std::string> strings;
    boost::split(strings, body, boost::is_any_of(
                 std::string(1, (char)0x3)),
                 boost::token_compress_on);
    Serializable *type = serializableFactory.create(strings[0]);
    std::vector<char> editedBody;
    std::copy((body.begin() + strings[0].length() + 1), body.end(),
            std::back_inserter(editedBody));
    boost::any object = type->deserialize(editedBody);
    return map[strings[0]](data, object);
}

} /* namespace util */
} /* namespace accord */
