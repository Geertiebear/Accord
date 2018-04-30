#include <accordserver/database/Database.h>

namespace accord {
namespace database {

Database::Database(std::string name, std::string address, std::string user,
                   std::string password, int port)
    : name(name),
      address(address),
      user(user),
      password(password),
      port(port)
{

}

Database::~Database()
{
    connection.disconnect();
}

int Database::connect()
{
    return connection.connect(name.c_str(), address.c_str(), user.c_str(),
                       password.c_str(), port);
}

} /* namespace database */
} /* namespace accord */
