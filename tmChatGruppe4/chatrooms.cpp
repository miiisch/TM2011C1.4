#include "chatrooms.h"
#include "chatroom.h"
#include <QPair>
#include <QMap>
#include <QDebug>

ChatRooms::ChatRooms() :
    chatRoomIdCounter(1), localClientId(0)
{
}

void ChatRooms::newData(ChatSocket* socket, DataElement data, quint32 userId)
{
    quint32 chatRoomId = data.chatRoomIdentifier();
    if (!chatRooms.contains(chatRoomId))
    {
        qDebug() << "Server received Message for unknown Channel" << chatRoomId;
        return;
    }
    chatRooms[chatRoomId]->newData(socket, data, userId);
}

QMap<quint32, QPair<QString, quint32> > ChatRooms::chatRoomsInfo()
{
    QMap<quint32, QPair<QString, quint32> > infos;
    foreach(ChatRoom* room, chatRooms)
    {
        infos[room->id()] = QPair<QString, quint32>(room->name(), room->chatRoomUsersCount());
    }
    return infos;
}

void ChatRooms::addChatRoom(QString name, bool denyAll)
{
    while(chatRooms.contains(chatRoomIdCounter))
    {
        chatRoomIdCounter++;
    }
    chatRooms.insert(chatRoomIdCounter, new ChatRoom(chatRoomIdCounter, name, denyAll));
    chatRooms[chatRoomIdCounter]->registerLocalClient(localClientId);
}

void ChatRooms::userConnectionLost(quint32 uid)
{
    foreach(ChatRoom* room, chatRooms)
        room->userConnectionLost(uid);
}

void ChatRooms::setDenyAll(bool deny)
{
    foreach(ChatRoom* room, chatRooms)
        room->denyAll(deny);
}

void ChatRooms::registerLocalClient(quint32 clientId)
{
    localClientId = clientId;
    foreach(quint32 channelId, chatRooms.keys())
    {
        chatRooms[channelId]->registerLocalClient(clientId);
    }
}

void ChatRooms::removeChatRoom(uint id, QString reason)
{
    if (chatRooms.contains(id))
    {
        chatRooms[id]->close(reason);
        chatRooms.remove(id);
    }
}
