#include <accordserver/database/Database.h>

namespace accord {
namespace database {

Database::Database(const DatabaseOptions &options)
    : options(options)
{

}

Database::~Database()
{
    connection.disconnect();
}

int Database::connect()
{
    return connection.connect(options.name.c_str(), options.address.c_str(),
                              options.user.c_str(),
                       options.password.c_str(), options.port);
}

} /* namespace database */
} /* namespace accord */
