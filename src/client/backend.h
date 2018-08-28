#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <QSslSocket>
#include <QUrl>
#include <QFile>
#include <QMap>
#include <QCache>
#include <QVariant>
#include <QQmlContext>
#include <string>

#include <accordshared/types/Request.h>
#include <accordshared/types/Database.h>
#include <accordshared/network/PacketData.h>
#include <accordshared/network/PacketDecoder.h>
#include <accordshared/network/PacketHandler.h>
#include <accordshared/util/Serialization.h>

class BackEnd;

struct PacketBuffer {
    accord::network::PacketId id;
    uint64_t length;
    std::vector<char> buffer;
};

struct Server : public PacketData {
    Server(BackEnd &backend) : backend(backend) { }
    QByteArray token;
    BackEnd &backend;
};

class AddCommunity {
public:
    AddCommunity() { }
    AddCommunity(QString name, QVector<char> profilepic)
        : name(name), profilepic(profilepic) { }
    QString name;
    QVector<char> profilepic;

    accord::types::AddCommunity toShared()
    {
        return accord::types::AddCommunity(name.toStdString(), profilepic.toStdVector());
    }
};

class CommunitiesTable : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString id MEMBER id CONSTANT)
    Q_PROPERTY(QString name MEMBER name CONSTANT)
    Q_PROPERTY(int members MEMBER members CONSTANT)
    Q_PROPERTY(int channels MEMBER channels CONSTANT)
public:
    CommunitiesTable() { }
    QString id; /* QML doesn't have support for 64 bit ints so store it in string */
    QString name;
    QByteArray profilepic;
    int members;
    int channels;

    void fromShared(accord::types::CommunitiesTable &table)
    {
        id = QString::fromStdString(std::to_string(table.id));
        name = QString::fromStdString(table.name);
        profilepic = QByteArray(&table.profilepic[0], table.profilepic.size());
        members = table.members;
        channels = table.channels;
    }
};

Q_DECLARE_METATYPE(CommunitiesTable*)

class ChannelsTable : public QObject {
  Q_OBJECT
public:
    ChannelsTable() { }
    QString id, community, name, description;

    void fromShared(const accord::types::ChannelsTable &table)
    {
        id = QString::fromStdString(std::to_string(table.id));
        community = QString::fromStdString(std::to_string(table.community));
        name = QString::fromStdString(table.name);
        description = QString::fromStdString(table.description);
    }
};

Q_DECLARE_METATYPE(ChannelsTable*)

/* generic object for displaying a list in QML
 * from a QVariantList */
class DataList : public QObject {
  Q_OBJECT
  Q_PROPERTY(QVariantList data MEMBER data NOTIFY dataChanged)
public:
    DataList(QObject *parent = nullptr) : QObject(parent) { }

    template<typename T>
    void fromVector(std::vector<T> list)
    {
        for (T item : list)
            data.append(QVariant::fromValue(item));
    }
    QVariantList data;
signals:
    void dataChanged();
};

class BackEnd : public QObject {
    Q_OBJECT
public:
    virtual ~BackEnd() {}
    explicit BackEnd(QObject *parent = nullptr);
    BackEnd(QQmlContext *ctx, QObject *parent = nullptr);

    static bool noopPacket(const std::vector<char> &body, PacketData *data);
    static bool receiveErrorPacket(const std::vector<char> &body, PacketData *data);
    static bool receiveTokenPacket(const std::vector<char> &body, PacketData *data);
    static bool receiveSerializePacket(const std::vector<char> &body, PacketData *data);

    static bool handleCommunitiesTable(PacketData *data, const std::vector<char> &body);
    static bool handleChannelsTable(PacketData *data, const std::vector<char> &body);

    DataList communitiesList;
    QQmlContext *qmlContext;
signals:
    void authenticated();
    void failedAuthenticated();
    void failedRegistered();
    void communityReady(QVariant table);
    void communityProfilepic(quint64, QByteArray data);
public slots:
    bool authenticate(QString email, QString password);
    bool regist(QString name, QString email, QString password); //register but register is a keyword >.>
    void addCommunity(QString name, QUrl file);
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
    QVector<char> readFile(QFile &file);
    void handleFileError(QUrl file);
    bool isPartialPacket;
    PacketBuffer partialPacket;

    void handlePartialPacket();
};

#endif // BACKEND_H
