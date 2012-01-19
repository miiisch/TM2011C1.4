#include "clientchatrooms.h"
#include <QTcpSocket>
#include "client.h"

typedef QHash<quint32,ClientChatRoom*> ClientChatRoomHash;

ClientChatRooms::ClientChatRooms()
{
}

void ClientChatRooms::newData(DataElement data, QHostAddress address, quint32 userId)
{
    chatRooms[address][data.chatRoomIdentifier()]->newData(data, userId);
}

ChatSocket * ClientChatRooms::serverConnection(QHostAddress ip, quint16 port)
{
    if (!chatRooms[ip].isEmpty())
        return chatRooms[ip].values()[0]->socket();

    QTcpSocket * socket = new QTcpSocket();
    socket->connectToHost(ip, port);
    socket->waitForConnected();
    return new ChatSocket(socket, 0);
}

void ClientChatRooms::addChatRoom(ChatSocket* socket, quint32 id, quint32 userId, QString name, QList<UserInfo> userInfo)
{
    chatRooms[socket->ip()][id] = new ClientChatRoom(socket, id, name, userId, userInfo);
}

void ClientChatRooms::sendKeepAlives()
{
    QHash<QHostAddress, QHash<quint32, ClientChatRoom*> >::iterator it1;
    for (it1 = chatRooms.begin(); it1 != chatRooms.begin(); ++it1)
    {
        QHostAddress k1 = it1.key();
        QHash<quint32, ClientChatRoom*> v1 = it1.value();
        QHash<quint32, ClientChatRoom*>::iterator it2;
        for (it2 = v1.begin(); it2 != v1.end(); ++it2)
        {
            ClientChatRoom * chatRoom = it2.value();
            if(chatRoom->socket()->timeOutCounter() > 5)
            {
                chatRooms[k1].remove(chatRoom->id());
            } else {
                chatRoom->socket()->incrementTimeOutCounter();
                chatRoom->socket()->send(DataElement(0,1,0,0,0), false);
            }
        }
    }
}
