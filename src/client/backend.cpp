#include "backend.h"
#include "util.h"
#include <QDebug>

#include <iostream>

#include <accordshared/network/packet/AuthPacket.h>
#include <accordshared/network/packet/RegisterPacket.h>
#include <accordshared/network/packet/RequestDataPacket.h>
#include <accordshared/network/PacketDecoder.h>
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
    { accord::network::COMMUNITIES_TABLE_REQUEST, &BackEnd::handleCommunitiesTable }
};

BackEnd::BackEnd(QObject *parent) : QObject(parent), state(*this)
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
    return socket.write(data);
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
    QByteArray received = socket.readAll();
    std::vector<char> data = Util::convertQtToVectorChar(received);
    accord::network::PacketDecoder::receivePacket(data, &state);
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
    accord::network::RequestDataPacket packet;
    auto msg = packet.construct(0);
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
    accord::types::CommunitiesTable table = accord::util::Serialization::
            deserealize<accord::types::CommunitiesTable>(body);
    CommunitiesTable ownTable;
    ownTable.fromShared(table);
    Server *server = (Server*) data;
    server->backend.communityReady(QVariant::fromValue(&ownTable));
    return true;
}

void BackEnd::addCommunity(QString name, QUrl file)
{
    qDebug() << name << file;
}
