#ifndef UTIL_H
#define UTIL_H

#include <QByteArray>
#include <vector>

class Util {
public:
    static QByteArray convertCharVectorToQt(std::vector<char> &data);
};

#endif // UTIL_H
