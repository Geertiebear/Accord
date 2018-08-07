#ifndef ACCORD_UTIL_FACTORY_H
#define ACCORD_UTIL_FACTORY_H

#include <functional>
#include <map>
#include <string>

namespace accord {
namespace util {

template<class T>
class Factory {
    typedef std::function<T*()> Creator;
    typedef std::map<std::string, Creator> Creators;
    Creators creators;

public:
    void regist(const std::string name, const Creator creator)
    {
        creators.insert(std::make_pair(name, creator));
    }

    T *create(const std::string name)
    {
        return creators[name]();
    }
};

} /* namespace util */
} /* namespace accord */

#endif
