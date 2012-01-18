#include "clientchatrooms.h"
#include <QTcpSocket>
#include "client.h"

ClientChatRooms::ClientChatRooms()
{
}

void ClientChatRooms::newData(DataElement data, quint32 userId)
{
    chatRooms[data.chatRoomIdentifier()]->newData(data, userId);
}

ChatSocket * ClientChatRooms::serverConnection(QHostAddress ip, quint16 port)
{
    foreach(ClientChatRoom* chatRoom, chatRooms.values())
    {
        if(chatRoom->socket()->ip() == ip)
        {
            return chatRoom->socket();
        }
    }
    QTcpSocket * socket = new QTcpSocket();
    socket->connectToHost(ip, port);
    socket->waitForConnected();
    return new ChatSocket(socket, 0);
}

void ClientChatRooms::addChatRoom(ChatSocket* socket, quint32 id, quint32 userId, QString name, QList<UserInfo> userInfo)
{
    chatRooms.insert(id, new ClientChatRoom(socket, id, name, userId, userInfo));
}

void ClientChatRooms::sendKeepAlives()
{
    foreach(ClientChatRoom * chatRoom, chatRooms.values())
    {
        if(chatRoom->socket()->timeOutCounter() > 5)
        {
            chatRooms.remove(chatRoom->id());
        } else {
            chatRoom->socket()->incrementTimeOutCounter();
            chatRoom->socket()->send(DataElement(0,1,0,0,0), false);
        }
    }
}
