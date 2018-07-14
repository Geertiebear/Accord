#ifndef UTIL_H
#define UTIL_H

#include <QByteArray>
#include <vector>

class Util {
public:
    static QByteArray convertCharVectorToQt(const std::vector<char> &data);
    static std::vector<char> convertQtToVectorChar(const QByteArray &data);
};

#endif // UTIL_H
