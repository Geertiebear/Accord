#include "backend.h"
#include "util.h"
#include <QDebug>

#include <iostream>

#include <accordshared/network/packet/AuthPacket.h>

BackEnd::BackEnd(QObject *parent) : QObject(parent)
{
    socket.setPeerVerifyMode(QSslSocket::QueryPeer);
    connect();
}

QByteArray BackEnd::read(qint64 maxSize)
{
    QByteArray res;
    if (!connected)
        connect();
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
        connect();
    return socket.write(data);
}

void BackEnd::connect()
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
