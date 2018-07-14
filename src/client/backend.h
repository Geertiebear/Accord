#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <QSslSocket>

#include <accordshared/network/PacketData.h>
#include <accordshared/network/PacketHandler.h>

struct Server : public PacketData {
    QString token;
};

class BackEnd : public QObject {
    Q_OBJECT
public:
    virtual ~BackEnd() {}
    explicit BackEnd(QObject *parent = nullptr);

    static bool noopPacket(const std::vector<char> &body, PacketData *data);
    static bool receiveTokenPacket(const std::vector<char> &body, PacketData *data);
signals:

public slots:
    bool authenticate(QString email, QString password);
    void readyRead();
private:
    static std::vector<accord::network::ReceiveHandler> handlers;
    qint64 write(const QByteArray &data);
    QByteArray read(qint64 maxSize);
    void doConnect();
    QSslSocket socket;
    bool connected;
    Server state;
};

#endif // BACKEND_H
