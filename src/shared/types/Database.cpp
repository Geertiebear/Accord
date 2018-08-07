#include <accordshared/types/Database.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace accord {
namespace types {

std::vector<char> CommunitiesTable::serialize()
{
    std::vector<char> res;
    std::string type = "CommunitiesTable";
    std::copy(type.begin(), type.end(), std::back_inserter(res));
    res.push_back(0x3);
    res.push_back(id); res.push_back(0x3);
    std::copy(name.begin(), name.end(), std::back_inserter(res));
    res.push_back(0x3);
    std::copy(profilepic.begin(), profilepic.end(), std::back_inserter(res));
    res.push_back(0x3);
    res.push_back(members); res.push_back(0x3);
    res.push_back(channels);
    return res;
}

boost::any CommunitiesTable::deserialize(const std::vector<char> &data)
{
    //split up the data
    std::vector<std::string> strings;
    boost::split(strings, data, boost::is_any_of(
                 std::string(1, (char)0x3)),
                 boost::token_compress_on);
    //prepare all of the parameters
    uint64_t id = std::stoi(strings[0]);
    std::string name = strings[1];
    std::string profilepic = strings[2];
    int members = std::stoi(strings[3]);
    int channels = std::stoi(strings[4]);
    return CommunitiesTable(id, name, profilepic, members, channels);
}

} /* namespace types */
} /* namespace accord */
