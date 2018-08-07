#include <accordshared/util/Serialization.h>
#include <accordshared/types/Database.h>

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

} /* namespace util */
} /* namespace accord */
