#include "util.h"

QByteArray Util::convertCharVectorToQt(std::vector<char> &data)
{
    return QByteArray::fromRawData(&data[0], data.size());
}
