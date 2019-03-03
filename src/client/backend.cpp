#include "backend.h"
#include "util.h"
#include <QDebug>

#include<algorithm>
#include <iostream>
#include <chrono>
#include <QErrorMessage>
#include <QApplication>
#include <QClipboard>

#include<accordshared/network/Packet.h>
#include <accordshared/network/packet/AuthPacket.h>
#include <accordshared/network/packet/SerializationPacket.h>
#include <accordshared/network/packet/RegisterPacket.h>
#include <accordshared/network/packet/KeepAlivePacket.h>
#include <accordshared/network/PacketDecoder.h>
#include <accordshared/types/Request.h>
#include <accordshared/error/ErrorCodes.h>
#include <accordshared/types/Return.h>
#include <accordshared/util/BinUtil.h>
#include <accordshared/types/Database.h>

std::vector<accord::network::ReceiveHandler> BackEnd::handlers = {
    &BackEnd::noopPacket,
    &BackEnd::receiveErrorPacket,
    &BackEnd::receiveDisconnectPacket,
    &BackEnd::noopPacket,
    &BackEnd::noopPacket,
    &BackEnd::receiveTokenPacket,
    &BackEnd::receiveSerializePacket,
    &BackEnd::receiveKeepAlivePacket
};

accord::util::FunctionMap BackEnd::serializationMap = {
    { accord::types::COMMUNITIES_TABLE_REQUEST, &BackEnd::handleCommunitiesTable },
    { accord::types::CHANNELS_REQUEST, &BackEnd::handleChannelsTable },
    { accord::types::AUTH_REQUEST, &BackEnd::handleAuth },
    { accord::types::COMMUNITY_TABLE_REQUEST, &BackEnd::handleCommunityTable },
    { accord::types::MESSAGES_REQUEST, &BackEnd::handleMessages },
    { accord::types::MESSAGE_REQUEST, &BackEnd::handleMessage },
    { accord::types::MESSAGE_SUCCESS, &BackEnd::handleMessageSuccess },
    { accord::types::CHANNEL_REQUEST, &BackEnd::handleChannel },
    { accord::types::USER_REQUEST, &BackEnd::handleUser },
    { accord::types::INVITE_REQUEST, &BackEnd::handleInvite },
    { accord::types::ONLINE_LIST_REQUEST, &BackEnd::handleOnlineList }
};

BackEnd::BackEnd(QObject *parent) : QObject(parent), state(*this)
  ,isPartialPacket(false)
{
    socket.setPeerVerifyMode(QSslSocket::QueryPeer);
    accord::network::PacketDecoder::init();
    accord::network::PacketHandler::init(handlers);
    QObject::connect(&socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    QObject::connect(&socket, SIGNAL(stateChanged(
                                         QAbstractSocket::SocketState)),
                     this, SLOT(stateChanged(QAbstractSocket::SocketState)));
    QObject::connect(&reconnectTimer, SIGNAL(timeout()), this, SLOT(doConnect()));
    QObject::connect(&eventTimer, SIGNAL(timeout()), this, SLOT(onEventTimer()));
    eventTimer.start(2000);
    doConnect();
}

BackEnd::BackEnd(QQmlContext *ctx, QObject *parent)
    : BackEnd(parent)
{
    qmlContext = ctx;
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
    socket.connectToHostEncrypted("chat.bearservers.net", 6524);
    qDebug() << "connecting!";
    if (!socket.waitForEncrypted()) {
        qDebug() << socket.errorString();
        return;
    }
    connected = true;
    timeSinceKeepAlive = 0;
    if (reconnectTimer.isActive())
        reconnectTimer.stop();
}

void BackEnd::onEventTimer()
{
    checkPendingMessages();
    checkConnected();
}

void BackEnd::checkPendingMessages()
{
    for (QVariant variant : messagesMap) {
        auto &messagesList = variant.value<DataList*>()->data;
        auto it = messagesList.end();
        while (it != messagesList.begin()) {
            it--;
            const auto variant = *it;
            const auto message = variant.value<MessagesTable*>();
            if (message->pending && !message->failure) {
                if (message->hasTimedOut()) {
                    const auto newMessage = new MessagesTable(message);
                    *it = QVariant::fromValue(newMessage);
                }
            }
        }
    }
    qmlContext->setContextProperty("messagesMap", messagesMap);
}

void BackEnd::checkConnected()
{
    if (timeSinceKeepAlive >= 4 && connected) {
        connected = false;
        socket.abort();
        return;
    } else if (!connected) return;

    accord::network::KeepAlivePacket packet;
    const auto msg = packet.construct();
    write(Util::convertCharVectorToQt(msg));
    timeSinceKeepAlive++;
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

    qDebug() << "Packet id: " << id;
    qDebug() << "Packet length: " << length;
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

void BackEnd::stateChanged(QAbstractSocket::SocketState state)
{
    if (state == QAbstractSocket::UnconnectedState) {
        connected = false;
        /* TODO: actually make this something useful and not just
         * spamming the debug log all the time */
        reconnectTimer.start(10000);
    }
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
    qDebug() << "Got error: " << error;

    switch (error) {
        case accord::AUTH_ERR: {
            server->backend.failedAuthenticated();
            break;
        }
        case accord::REGIST_ERR: {
            server->backend.failedRegistered();
            break;
        }
        case accord::NOT_LOGGED_IN_ERR: {
            if (server->token.token.empty()) {
                server->backend.failedAuthenticated();
                break;
            }
            accord::network::SerializationPacket packet;
            const auto json = accord::util::Serialization::serialize(
                        server->token);
            const auto msg = packet.construct(accord::types::
                                              AUTH_WITH_TOKEN_REQUEST, json);
            QByteArray array = Util::convertCharVectorToQt(msg);
            server->backend.write(array);
            server->backend.retryFailedRequest();
            break;
        }
        case accord::ALREADY_IN_ERR: {
            server->backend.alreadyInCommunity();
            break;
        }
    }

    return false;
}

bool BackEnd::receiveDisconnectPacket(const std::vector<char> &body, PacketData *data)
{
    auto server = (Server*) data;
    server->backend.connected = false;
    return true;
}

bool BackEnd::receiveTokenPacket(const std::vector<char> &body, PacketData *data)
{
    Server *server = (Server*) data;
    //server->token = Util::convertCharVectorToQt(body);
    server->backend.authenticated();

    //TODO: temp
    accord::network::SerializationPacket packet;
    accord::types::Communities request(server->token.token);
    const auto json = accord::util::Serialization::serialize(request);
    auto msg = packet.construct(accord::types::COMMUNITIES_TABLE_REQUEST,
                                json);
    server->backend.write(Util::convertCharVectorToQt(msg));
    return true;
}

bool BackEnd::receiveKeepAlivePacket(const std::vector<char> &body,
                                     PacketData *data)
{
    (void)body; /* supress not used warnings */
    auto server = (Server*) data;
    /* just reset the timer */
    server->backend.timeSinceKeepAlive = 0;
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

bool BackEnd::loadChannels(QString id)
{
    quint64 intId = id.toULongLong();
    accord::types::Channels request(intId, state.token.token);
    accord::network::SerializationPacket packet;
    const auto json = accord::util::Serialization::serialize(request);
    const auto msg = packet.construct(accord::types::CHANNELS_REQUEST, json);
    lastRequest = msg;
    return write(Util::convertCharVectorToQt(msg));
}

bool BackEnd::loadChannel(QString id)
{
    bool ret = true;
    ret = loadMessages(id);
    if (!ret)
        return ret;
    ret = loadOnlineList(id);
    return ret;
}

bool BackEnd::loadMessages(QString id)
{
    quint64 intId = id.toULongLong();
    accord::types::Messages request(intId, state.token.token);
    accord::network::SerializationPacket packet;
    const auto json = accord::util::Serialization::serialize(request);
    const auto msg = packet.construct(accord::types::MESSAGES_REQUEST, json);
    lastRequest = msg;
    return write(Util::convertCharVectorToQt(msg));
}

bool BackEnd::loadOnlineList(QString id)
{
    quint64 intId = id.toULongLong();
    accord::types::OnlineList request(intId, state.token.token);
    accord::network::SerializationPacket packet;
    const auto json = accord::util::Serialization::serialize(request);
    const auto msg = packet.construct(accord::types::ONLINE_LIST_REQUEST, json);
    lastRequest = msg;
    return write(Util::convertCharVectorToQt(msg));
}

bool BackEnd::loadUser(QString id)
{
    quint64 intId = id.toULongLong();
    auto it = std::find(pendingUserRequests.begin(),
                        pendingUserRequests.end(),
                        intId);
    if (it != pendingUserRequests.end())
        return false;
    accord::types::User request(intId, state.token.token);
    accord::network::SerializationPacket packet;
    const auto json = accord::util::Serialization::serialize(request);
    const auto msg = packet.construct(accord::types::USER_REQUEST, json);
    lastRequest = msg;
    pendingUserRequests.push_back(intId);
    return write(Util::convertCharVectorToQt(msg));
}

void BackEnd::addChannel(QString name, QString description, QString community)
{
    uint64_t communityInt = community.toULongLong();
    accord::types::AddChannel request(communityInt,
                                      name.toStdString(),
                                      description.toStdString(),
                                      state.token.token);
    accord::network::SerializationPacket packet;
    const auto json = accord::util::Serialization::serialize(request);
    const auto msg = packet.construct(accord::types::ADD_CHANNEL_REQUEST, json);
    lastRequest = msg;
    write(Util::convertCharVectorToQt(msg));
}

bool BackEnd::requestInvite(QString id)
{
    auto idInt = id.toULongLong();
    const accord::types::Invite request(idInt, state.token.token);
    accord::network::SerializationPacket packet;
    const auto json = accord::util::Serialization::serialize(request);
    const auto msg = packet.construct(accord::types::INVITE_REQUEST, json);
    lastRequest = msg;
    return write(Util::convertCharVectorToQt(msg));
}

void BackEnd::stringToClipboard(QString string)
{
    QApplication::clipboard()->setText(string);
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
    std::vector<CommunitiesTable*> ownTables;
    for (auto &table : tables) {
        auto ownTable = new CommunitiesTable;
        ownTable->fromShared(table);
        server->backend.communityProfilepic(
                    table.id, ownTable->profilepic);
        ownTable->profilepic.clear();
        ownTables.push_back(ownTable);
    }
    auto &list = server->backend.communitiesList;
    list.data.clear();
    list.fromVector(ownTables);
    server->backend.qmlContext->setContextProperty("communitiesList",
                                                   &list);
    return true;
}

bool BackEnd::handleChannelsTable(PacketData *data, const std::vector<char> &body)
{
    const auto res = accord::util::Serialization::
            deserealize<accord::types::ChannelsReturn>(body);
    const auto &tables = res.channels;
    const auto &community = res.community;
    const auto communityString = QString::fromStdString(std::to_string(community));
    auto server = (Server*) data;
    auto &channelsMap = server->backend.channelsMap;

    channelsMap.insert(communityString, QVariant::fromValue(new DataList()));

    if (tables.empty()) {
        server->backend.qmlContext->setContextProperty("channelsMap",
                                                channelsMap);
        return false;
    }

    std::vector<ChannelsTable*> ownTables;
    for (auto &table : tables) {
        auto ownTable = new ChannelsTable;
        ownTable->fromShared(table);
        ownTables.push_back(ownTable);
    }

    auto list = channelsMap[communityString];
    list.value<DataList*>()->fromVector(ownTables);
    server->backend.qmlContext->setContextProperty("channelsMap",
                                            channelsMap);
    return true;
}

bool BackEnd::handleAuth(PacketData *data, const std::vector<char> &body)
{
    const auto token = accord::util::Serialization::deserealize<
            accord::types::Token>(body);
    auto server = (Server*) data;
    server->token = token;

    //TODO: temp
    receiveTokenPacket(std::vector<char>(), data);
    return true;
}

bool BackEnd::handleCommunityTable(PacketData *data, const std::vector<char> &body)
{
    auto server = (Server*) data;
    const auto table = accord::util::Serialization::deserealize<
            accord::types::CommunitiesTable>(body);
    auto ownTable = new CommunitiesTable;
    ownTable->fromShared(table);
    server->backend.communityProfilepic(
                table.id, ownTable->profilepic);
    server->backend.communitiesList.data.append(
                QVariant::fromValue(ownTable));
    server->backend.qmlContext->setContextProperty("communitiesList",
                                 &server->backend.communitiesList);
    return true;
}

bool BackEnd::handleMessages(PacketData *data, const std::vector<char> &body)
{
    const auto res = accord::util::Serialization::
            deserealize<accord::types::MessagesReturn>(body);
    const auto &tables = res.messages;
    const auto &channel = res.channel;
    const auto channelString = QString::fromStdString(std::to_string(channel));
    auto server = (Server*) data;
    auto &messagesMap = server->backend.messagesMap;

    messagesMap.insert(channelString, QVariant::fromValue(new DataList()));

    if (tables.empty()) {
        server->backend.qmlContext->setContextProperty("messagesMap",
                                                messagesMap);
        return false;
    }

    std::vector<MessagesTable*> ownTables;
    for (auto &table : tables) {
        auto ownTable = new MessagesTable;
        ownTable->fromShared(table);
        ownTables.push_back(ownTable);
    }

    auto list = messagesMap[channelString];
    list.value<DataList*>()->fromVector(ownTables);
    server->backend.qmlContext->setContextProperty("messagesMap",
                                            messagesMap);
    return true;
}

bool BackEnd::handleMessage(PacketData *data, const std::vector<char> &body)
{
    auto server = (Server*) data;
    const auto table = accord::util::Serialization::deserealize<
            accord::types::MessagesTable>(body);
    const auto &channel = table.channel;
    const auto channelString = QString::fromStdString(std::to_string(channel));
    auto ownTable = new MessagesTable;
    ownTable->fromShared(table);
    /* server->backend.communityProfilepic(
                table.id, ownTable->profilepic); */

    auto &messagesMap = server->backend.messagesMap;
    if (!server->backend.messagesMap[channelString].value<DataList*>())
        server->backend.messagesMap.insert(channelString,
                                           QVariant::fromValue(new DataList()));
    auto &messagesList = messagesMap[channelString].value<DataList*>()->data;
    auto it = messagesList.end();
    while (it != messagesList.begin() && !messagesList.empty()) {
        it--;
        const auto variant = *it;
        const auto message = variant.value<MessagesTable*>();
        if (message->isPendingOf(ownTable))
            messagesList.erase(it);
    }

    server->backend.messagesMap[channelString].
            value<DataList*>()->data.append(
                QVariant::fromValue(ownTable));
    server->backend.qmlContext->setContextProperty("messagesMap",
                                 server->backend.messagesMap);
    return true;
}
bool BackEnd::handleMessageSuccess(PacketData *data, const std::vector<char> &body)
{
    /* TODO wtf is this actually for?*/
    return true;
}

bool BackEnd::handleChannel(PacketData *data, const std::vector<char> &body)
{
    auto server = (Server*) data;
    const auto ret = accord::util::Serialization::deserealize<
            accord::types::ChannelsTable>(body);
    const auto &community = ret.community;
    const auto communityString = QString::fromStdString(std::to_string(
                                                            community));
    auto ownTable = new ChannelsTable;
    ownTable->fromShared(ret);
    server->backend.channelsMap[communityString].
            value<DataList*>()->data.append(
                QVariant::fromValue(ownTable));
    server->backend.qmlContext->setContextProperty("channelsMap",
                                 server->backend.channelsMap);
    return true;
}

bool BackEnd::handleUser(PacketData *data, const std::vector<char> &body)
{
    auto server = (Server*) data;
    const auto ret = accord::util::Serialization::deserealize<
            accord::types::UserData>(body);
    auto ownTable = new UserData(ret);
    const auto &id = ret.id;
    const auto idString = QString::fromStdString(std::to_string(id));

    auto it = std::find(
                server->backend.pendingUserRequests.begin(),
                server->backend.pendingUserRequests.end(), id);
    if (it != server->backend.pendingUserRequests.end())
        server->backend.pendingUserRequests.erase(it);

    server->backend.userMap[idString] = QVariant::fromValue(ownTable);
    server->backend.qmlContext->setContextProperty("userMap",
                                                   server->backend.userMap);
    return true;
}

bool BackEnd::handleInvite(PacketData *data, const std::vector<char> &body)
{
    auto server = (Server*) data;
    const auto ret = accord::util::Serialization::deserealize<
            accord::types::InviteRet>(body);
    const auto invite = QString::fromStdString(ret.invite);
    const auto id = QString::fromStdString(std::to_string(ret.id));
    server->backend.inviteReady(id, invite);
    return true;
}

bool BackEnd::handleOnlineList(PacketData *data, const std::vector<char> &body)
{
    auto server = (Server*) data;
    const auto ret = accord::util::Serialization::deserealize<
            accord::types::OnlineListRet>(body);

    std::list<UserData*> ownList;
    for (accord::types::UserData user : ret.list)
        ownList.push_back(new UserData(user));

    const auto channelString = QString::fromStdString(std::to_string(ret.id));
    server->backend.onlineMap.insert(channelString, QVariant::fromValue(new DataList()));
    auto list = server->backend.onlineMap[channelString];
    list.value<DataList*>()->fromList(ownList);
    server->backend.qmlContext->setContextProperty("onlineMap",
                                            server->backend.onlineMap);
    return true;
}

void BackEnd::retryFailedRequest()
{
    if (lastRequest.empty())
        return;
    write(Util::convertCharVectorToQt(lastRequest));
    lastRequest.clear();
}

void BackEnd::addCommunity(QString name, QUrl file)
{
    QFile fileObj(file.toLocalFile());
    QVector<char> buffer = readFile(fileObj);
    if (buffer.empty()) {
        handleFileError(file);
        return;
    }

    AddCommunity request(name, buffer,
                         QString::fromStdString(state.token.token));
    auto shared = request.toShared();
    auto data = accord::util::Serialization::serialize(shared);
    accord::network::SerializationPacket packet;
    auto msg = packet.construct(accord::types::ADD_COMMUNITY_REQUEST, data);
    lastRequest = msg;
    write(Util::convertCharVectorToQt(msg));
}

bool BackEnd::sendMessage(QString message, QString channel)
{   
    auto timestamp = std::chrono::system_clock::now().time_since_epoch()
            .count();
    auto channelInt = channel.toULongLong();

    auto tempMessage = new MessagesTable(channel, message,
                                     QString::fromStdString(
                                         std::to_string(timestamp)),
                                     true);
    if (!messagesMap[channel].value<DataList*>())
        messagesMap.insert(channel, QVariant::fromValue(new DataList()));
    messagesMap[channel].
            value<DataList*>()->data.append(
                QVariant::fromValue(tempMessage));
    qmlContext->setContextProperty("messagesMap",
                                 messagesMap);

    return sendMessageRequest(channelInt, message.toStdString(), timestamp,
                              state.token.token);
}

bool BackEnd::sendMessageRequest(uint64_t channelInt,
                                 const std::string &message,
                                 uint64_t timestamp, const std::string &token)
{
    auto request = accord::types::SendMessage(channelInt,
                                              message,
                                              timestamp,
                                              token);
    accord::network::SerializationPacket packet;
    const auto json = accord::util::Serialization::serialize(request);
    const auto msg = packet.construct(accord::types::SEND_MESSAGE_REQUEST,
                                      json);
    lastRequest = msg;
    return write(Util::convertCharVectorToQt(msg));
}

bool BackEnd::sendInvite(QString invite)
{
    accord::network::SerializationPacket packet;
    const auto json = accord::util::Serialization::serialize(invite.
                                                             toStdString());
    const auto msg = packet.construct(accord::types::SEND_INVITE_REQUEST,
                                      json);
    lastRequest = msg;
    return write(Util::convertCharVectorToQt(msg));
}

void BackEnd::retryMessage()
{
    const auto message = failedMessage.value<MessagesTable*>();
    auto variant = messagesMap[message->channel];
    auto dataList = variant.value<DataList*>();
    dataList->data.removeOne(failedMessage);
    sendMessage(message->contents, message->channel);
}

void BackEnd::setFailedMessage(QVariant variant)
{
    failedMessage = variant;
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
