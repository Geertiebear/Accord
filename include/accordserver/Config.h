#ifndef ACCORD_CONFIG_H
#define ACCORD_CONFIG_H

#include <string>
#include <yaml-cpp/yaml.h>
#include <accordserver/database/Database.h>

namespace accord {

struct Config {
    database::DatabaseOptions database;
};

class ConfigLoader {
public:
    ConfigLoader(std::string file);

    Config load();
private:
    std::string readString(YAML::Node *node, std::string name);
    int readInt(YAML::Node *node, std::string name);

    std::string file;
};

} /* namespace accord */

#endif
