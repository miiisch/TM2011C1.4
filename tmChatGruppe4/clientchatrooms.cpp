#include "clientchatrooms.h"
#include <QTcpSocket>
#include "client.h"

ClientChatRooms::ClientChatRooms() :
    _sendKeepalives(true), _checkKeepalives(true)
{
}

void ClientChatRooms::newData(DataElement data, QHostAddress address, quint32 userId)
{
    if (!chatRooms[address].contains(data.chatRoomIdentifier())) {
        qDebug() << "Client received message for unknown Channel "
                 << address << ", " << data.chatRoomIdentifier();
        return;
    }
    chatRooms[address][data.chatRoomIdentifier()]->newData(data, userId);
}

ChatSocket * ClientChatRooms::serverConnection(QHostAddress ip, quint16 port)
{
    if (!chatRooms[ip].isEmpty())
        return chatRooms[ip].values()[0]->socket();

    QTcpSocket * socket = new QTcpSocket();
    socket->connectToHost(ip, port);
    socket->waitForConnected();
    return new ChatSocket(socket, 0, true, this);
}

void ClientChatRooms::addChatRoom(ChatSocket* socket, quint32 id, quint32 userId, QString name)
{
    ClientChatRoom * room = new ClientChatRoom(socket, id, name, userId);
    connect(room, SIGNAL(closed(QHostAddress,quint32)), SLOT(roomClosed(QHostAddress,quint32)));
    chatRooms[socket->ip()][id] = room;
}

void ClientChatRooms::sendKeepAlives()
{
    QHash<QHostAddress, QHash<quint32, ClientChatRoom*> >::iterator it1;

    QList<QHostAddress> keys = chatRooms.keys();

    foreach (QHostAddress address, keys)
    {
        QHash<quint32, ClientChatRoom*> v1 = chatRooms[address];

        if (!v1.isEmpty())
        {
            ChatSocket * socket = v1[v1.keys()[0]]->socket();
            if(socket->timeOutCounter() > 5 && _checkKeepalives)
            {
                foreach(ClientChatRoom * c, chatRooms[address])
                    c->serverQuit();
                chatRooms.remove(address);
            } else {
                socket->incrementTimeOutCounter();
                if (_sendKeepalives)
                    socket->send(DataElement(0,1,0,0,0), false);
            }
        }
    }
}

void ClientChatRooms::activateChatRoom(QHostAddress address, quint32 id, QMap<quint32, UserInfo> userInfo)
{
    chatRooms[address][id]->activate(userInfo);
}

void ClientChatRooms::denyJoin(QHostAddress address, quint32 id, DataElement &data)
{
    chatRooms[address][id]->denyJoin(data);
}

bool ClientChatRooms::containsRoom(QHostAddress address, quint32 id)
{
    return chatRooms[address].contains(id);
}

void ClientChatRooms::roomClosed(QHostAddress address, quint32 id)
{
    chatRooms[address].remove(id);
}

void ClientChatRooms::activateSendKeepalives(bool x)
{
    _sendKeepalives = x;
}

void ClientChatRooms::activateCheckKeepalives(bool x)
{
    _checkKeepalives = x;
}
