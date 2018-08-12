#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <QSslSocket>

#include <accordshared/types/Database.h>
#include <accordshared/network/PacketData.h>
#include <accordshared/network/PacketHandler.h>
#include <accordshared/util/Serialization.h>

class BackEnd;

struct Server : public PacketData {
    Server(BackEnd &backend) : backend(backend) { }
    QByteArray token;
    BackEnd &backend;
};


class CommunitiesTable : public QObject {
    Q_OBJECT
    Q_PROPERTY(quint64 id MEMBER id)
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QVector<char> profilepic MEMBER profilepic)
    Q_PROPERTY(int members MEMBER members)
    Q_PROPERTY(int channels MEMBER channels)
public:
    CommunitiesTable() { }
    quint64 id;
    QString name;
    QVector<char> profilepic;
    int members;
    int channels;

    static void fromShared(CommunitiesTable &ret, accord::types::CommunitiesTable &table)
    {
        ret.id = table.id;
        ret.name = QString::fromStdString(table.name);
        ret.profilepic = QVector<char>::fromStdVector(std::vector<char>(table.profilepic.begin(), table.profilepic.end()));
        ret.members = table.members;
        ret.channels = table.channels;
    }
};

Q_DECLARE_METATYPE(CommunitiesTable*)


class BackEnd : public QObject {
    Q_OBJECT
public:
    virtual ~BackEnd() {}
    explicit BackEnd(QObject *parent = nullptr);

    static bool noopPacket(const std::vector<char> &body, PacketData *data);
    static bool receiveErrorPacket(const std::vector<char> &body, PacketData *data);
    static bool receiveTokenPacket(const std::vector<char> &body, PacketData *data);
    static bool receiveSerializePacket(const std::vector<char> &body, PacketData *data);

    static bool handleCommunitiesTable(PacketData *data, boost::any object);
signals:
    void authenticated();
    void failedAuthenticated();
    void failedRegistered();
    void communityReady(QVariant table);
public slots:
    bool authenticate(QString email, QString password);
    bool regist(QString name, QString email, QString password); //register but register is a keyword >.>
    void readyRead();
private:
    static std::vector<accord::network::ReceiveHandler> handlers;
    static accord::util::FunctionMap serializationMap;
    qint64 write(const QByteArray &data);
    QByteArray read(qint64 maxSize);
    void doConnect();
    QSslSocket socket;
    bool connected;
    Server state;
};

#endif // BACKEND_H
