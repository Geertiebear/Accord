#ifndef NETWORK_PACKET_H
#define NETWORK_PACKET_H

#include <vector>
#include <string>
#include <type_traits>

#include <accordshared/network/PacketHandler.h>
#include <accordshared/network/PacketData.h>
#include <accordshared/util/BinUtil.h>

namespace accord {
namespace network {

#define HEADER_SIZE 10

enum PacketIds {
	SEND_MESSAGE_PACKET = 0,
	ERROR_PACKET = 1,
	DISCONNECT_PACKET = 2,
    AUTH_PACKET = 3,
    REGISTER_PACKET = 4,
    TOKEN_PACKET = 5,
    SERIALIZATION_PACKET = 6,
    KEEP_ALIVE_PACKET = 7,
};

class Packet {
public:
    virtual ~Packet() { } //base class
	virtual Packet *clone() const = 0; //polymorphic construction
    virtual size_t getMaxSize() const = 0;
	virtual int getId() const {
		return -1;
	}

    bool receive(const std::vector<char> &body, PacketData *data) const
	{
		return PacketHandler::handle(getId(), body, data);
	}


    //helper functions for serializing different types of data
    template<typename T>
    static std::enable_if_t<std::is_same<T, uint16_t>::value> write(std::vector<char> &msg, T data)
    {
        uint8_t low = 0, high = 0;
        util::BinUtil::splitUint16(data, &low, &high);
        msg.push_back((char)low);
        msg.push_back((char)high);
    }

    template<typename T>
    static std::enable_if_t<std::is_same<T, std::string>::value> write(std::vector<char> &msg, T data)
    {
        std::copy(data.begin(), data.end(), std::back_inserter(msg));
    }

    template<typename T>
    static std::enable_if_t<std::is_same<T, std::vector<char>>::value> write(std::vector<char> &msg, T data)
    {
        std::copy(data.begin(), data.end(), std::back_inserter(msg));
    }

    template<typename T>
    static std::enable_if_t<std::is_same<T, PacketIds>::value> write(std::vector<char> &msg, T data)
    {
        write(msg, (uint16_t) data);
    }

    template<typename T>
    static std::enable_if_t<std::is_same<T, uint64_t>::value> write(std::vector<char> &msg, T data)
    {
        auto bytes = util::BinUtil::splitUint64(data);
        std::copy(bytes.begin(), bytes.end(), std::back_inserter(msg));
    }

    void writeHeader(PacketIds id, uint64_t length, std::vector<char> &msg)
    {
        write(msg, id);
        write(msg, length);
    }
};

} /* namespace network */
} /* namespace accord */

#endif
