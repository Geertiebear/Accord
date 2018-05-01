#include <accordserver/Config.h>
#include <accordserver/log/Logger.h>

#include <exception>

namespace accord {

ConfigLoader::ConfigLoader(std::string file) : file(file)
{ }

Config ConfigLoader::load()
{
    Config config;
    YAML::Node configFile = YAML::LoadFile(file);
    YAML::Node database = configFile["database"];
    YAML::Node openssl = configFile["openssl"];

    config.database.name = readString(&database, "name");
    config.database.address = readString(&database, "address");
    config.database.user = readString(&database, "user");
    config.database.password = readString(&database, "password");
    config.database.port = readInt(&database, "port");
    config.openssl.cert = readString(&openssl, "cert");
    config.openssl.pkey = readString(&openssl, "pkey");

    return config;
}

std::string ConfigLoader::readString(YAML::Node *node, std::string name)
{
    YAML::Node derefNode = *node;
    if (derefNode[name])
        return derefNode[name].as<std::string>();
    log::Logger::log(log::ERROR, "Missing config entry!");
    throw std::runtime_error(name);
}

int ConfigLoader::readInt(YAML::Node *node, std::string name)
{
    YAML::Node derefNode = *node;
    if (derefNode[name])
        return derefNode[name].as<int>();
    log::Logger::log(log::ERROR, "Missing config entry!");
    throw std::runtime_error(name);
}

} /* namespace accord */
