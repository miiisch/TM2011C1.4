#include "chatrooms.h"
#include "chatroom.h"
#include <QPair>
#include <QMap>

ChatRooms::ChatRooms() :
    chatRoomIdCounter(1)
{
}

void ChatRooms::newData(ChatSocket* socket, DataElement data, quint32 userId)
{
    quint32 chatRoomId = data.chatRoomIdentifier();
    chatRooms[chatRoomId]->newData(socket, data, userId);
}

QMap<quint32, QPair<QString, quint32> > ChatRooms::chatRoomsInfo()
{
    QMap<quint32, QPair<QString, quint32> > infos;
    foreach(ChatRoom* room, chatRooms)
    {
        infos[room->id()] = QPair<QString, quint32>(room->name(), room->chatRoomUsersCount()); //TDOO replace 0 with number of users in channel
    }
    return infos;
}

void ChatRooms::addChatRoom(QString name)
{
    while(chatRooms.contains(chatRoomIdCounter))
    {
        chatRoomIdCounter++;
    }
    chatRooms.insert(chatRoomIdCounter, new ChatRoom(chatRoomIdCounter, name));
}
