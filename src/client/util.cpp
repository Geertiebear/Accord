#include "util.h"

QByteArray Util::convertCharVectorToQt(const std::vector<char> &data)
{
    return QByteArray::fromRawData(&data[0], data.size());
}

std::vector<char> Util::convertQtToVectorChar(const QByteArray &data)
{
    return std::vector<char>(data.begin(), data.end());
}
