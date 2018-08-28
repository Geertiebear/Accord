#include "backend.h"
#include "util.h"
#include <QDebug>

#include <iostream>
#include <QErrorMessage>

#include<accordshared/network/Packet.h>
#include <accordshared/network/packet/AuthPacket.h>
#include <accordshared/network/packet/SerializationPacket.h>
#include <accordshared/network/packet/RegisterPacket.h>
#include <accordshared/network/PacketDecoder.h>
#include <accordshared/types/Request.h>
#include <accordshared/error/ErrorCodes.h>
#include <accordshared/util/BinUtil.h>
#include <accordshared/types/Database.h>

std::vector<accord::network::ReceiveHandler> BackEnd::handlers = {
    &BackEnd::noopPacket,
    &BackEnd::receiveErrorPacket,
    &BackEnd::noopPacket,
    &BackEnd::noopPacket,
    &BackEnd::noopPacket,
    &BackEnd::receiveTokenPacket,
    &BackEnd::receiveSerializePacket
};

accord::util::FunctionMap BackEnd::serializationMap = {
    { accord::types::COMMUNITIES_TABLE_REQUEST, &BackEnd::handleCommunitiesTable }
};

BackEnd::BackEnd(QObject *parent) : QObject(parent), state(*this)
  ,isPartialPacket(false)
{
    socket.setPeerVerifyMode(QSslSocket::QueryPeer);
    accord::network::PacketDecoder::init();
    accord::network::PacketHandler::init(handlers);
    QObject::connect(&socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    doConnect();
}

QByteArray BackEnd::read(qint64 maxSize)
{
    QByteArray res;
    if (!connected)
        doConnect();
    char *buffer = new char[maxSize];
    int ret = socket.read(buffer, maxSize);
    if (ret < 0)
        return res;
    res = QByteArray::fromRawData(buffer, ret);
    delete buffer;
    return res;
}

qint64 BackEnd::write(const QByteArray &data)
{
    if (!connected)
        doConnect();
    socket.write(data);
    socket.waitForBytesWritten();
    return 1;
}

void BackEnd::doConnect()
{
    socket.connectToHostEncrypted("localhost", 6524);
    if (!socket.waitForEncrypted()) {
        qDebug() << socket.errorString();
        return;
    }
    connected = true;
}

bool BackEnd::authenticate(QString email, QString password)
{
    accord::network::AuthPacket packet;
    std::vector<char> data = packet.construct(email.toStdString(),
                                               password.toStdString());
    QByteArray msg = Util::convertCharVectorToQt(data);
    return write(msg);
}

void BackEnd::readyRead()
{
    if (isPartialPacket) {
        handlePartialPacket();
        return;
    }

    std::vector<char> buffer;
    buffer.resize(HEADER_SIZE);
    socket.read(&buffer[0], HEADER_SIZE);

    const auto id = accord::util::BinUtil::assembleUint16(
                buffer[0], buffer[1]);
    const auto length = accord::util::BinUtil::assembleUint64(
                std::vector<uint8_t>(buffer.begin() + 2, buffer.end()));
    const uint64_t bufferSize = socket.bytesAvailable();

    qDebug() << length;
    if (length > bufferSize) {
        partialPacket.length = length;
        partialPacket.id = id;
        partialPacket.buffer.resize(bufferSize);
        socket.read(&partialPacket.buffer[0], bufferSize);
        isPartialPacket = true;
        return;
    }

    buffer.resize(length);
    socket.read(&buffer[0], length);
    accord::network::PacketDecoder::receivePacket(id, buffer, &state);

    if (socket.bytesAvailable())
        readyRead();
}

bool BackEnd::noopPacket(const std::vector<char> &body, PacketData *data)
{
    return true;
}

bool BackEnd::receiveErrorPacket(const std::vector<char> &body, PacketData *data)
{
    Server *server = (Server*) data;
    uint8_t low = body[0];
    uint8_t high = body[1];
    uint16_t error = accord::util::BinUtil::assembleUint16(low, high);

    switch (error) {
        case accord::AUTH_ERR:
            server->backend.failedAuthenticated();
            break;
        case accord::REGIST_ERR:
            server->backend.failedRegistered();
            break;
    }

    return false;
}

bool BackEnd::receiveTokenPacket(const std::vector<char> &body, PacketData *data)
{
    Server *server = (Server*) data;
    server->token = Util::convertCharVectorToQt(body);
    server->backend.authenticated();

    //TODO: temp
    accord::network::SerializationPacket packet;
    auto msg = packet.construct(accord::types::COMMUNITIES_TABLE_REQUEST,
                                std::string());
    server->backend.write(Util::convertCharVectorToQt(msg));
    return true;
}

bool BackEnd::regist(QString name, QString email, QString password)
{
    accord::network::RegisterPacket packet;
    std::vector<char> data = packet.construct(name.toStdString(),
                                              email.toStdString(),
                                              password.toStdString());
    QByteArray msg = Util::convertCharVectorToQt(data);
    return write(msg);
}

bool BackEnd::receiveSerializePacket(const std::vector<char> &body, PacketData *data)
{
    return accord::util::Serialization::receive(serializationMap, body, data);
}

bool BackEnd::handleCommunitiesTable(PacketData *data, const std::vector<char> &body)
{
    auto tables = accord::util::Serialization::
            deserealize<std::vector<
            accord::types::CommunitiesTable>>(body);
    Server *server = (Server*) data;
    for (auto &table : tables) {
        CommunitiesTable ownTable;
        ownTable.fromShared(table);
        server->backend.communityProfilepic(table.id, ownTable.profilepic);
        ownTable.profilepic.clear(); // we are done with it
        server->backend.communityReady(QVariant::fromValue(&ownTable));
    }
    return true;
}

void BackEnd::addCommunity(QString name, QUrl file)
{
    QFile fileObj(file.toLocalFile());
    QVector<char> buffer = readFile(fileObj);
    if (buffer.empty()) {
        handleFileError(file);
        return;
    }

    AddCommunity request(name, buffer);
    auto shared = request.toShared();
    auto data = accord::util::Serialization::serialize(shared);
    accord::network::SerializationPacket packet;
    auto msg = packet.construct(accord::types::ADD_COMMUNITY_REQUEST,
                                std::string(data.begin(), data.end()));
    write(Util::convertCharVectorToQt(msg));
}

QVector<char> BackEnd::readFile(QFile &file)
{
    if (!file.open(QIODevice::ReadOnly))
        return QVector<char>();
    QByteArray data = file.readAll(); //qt doesnt allow init from iterators wtf
    return QVector<char>::fromStdVector(std::vector<char>(data.begin(), data.end()));
}

void BackEnd::handleFileError(QUrl file)
{
    QErrorMessage msg;
    msg.showMessage("Failed to read file: " + file.toLocalFile());
}

void BackEnd::handlePartialPacket()
{
    const auto &length = partialPacket.length;
    const auto &id = partialPacket.id;
    auto &buffer = partialPacket.buffer;
    auto currentBufferSize = buffer.size();
    const uint64_t bufferSize = socket.bytesAvailable();
    const auto toRead = std::min(length - currentBufferSize, bufferSize);

    buffer.resize(toRead + currentBufferSize);
    socket.read(&buffer[currentBufferSize], toRead);

    currentBufferSize = buffer.size();
    if (length > currentBufferSize)
        return;

    isPartialPacket = false;
    accord::network::PacketDecoder::receivePacket(id, buffer, &state);
}
