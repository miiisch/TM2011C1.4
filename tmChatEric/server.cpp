#include "server.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include "chatsocket.h"
#include <QDebug>
#include "dataelement.h"
#include <QTimer>

Server::Server(QString debug, QObject *parent) :
    QObject(parent), tcpServer(new QTcpServer()), userIdCounter(1)
{
    chatRooms = new ChatRooms();
    QUdpSocket * socket = new QUdpSocket;
    socket->bind(13167);
    udpChatSocket = new ChatSocket(socket);
    connect(udpChatSocket,SIGNAL(newUdpData(DataElement,QHostAddress*)),SLOT(readBroadCast(DataElement,QHostAddress*)));
    tcpServer->listen();
    port = tcpServer->serverPort();
    connect(tcpServer,SIGNAL(newConnection()),SLOT(newConnection()));

    //every 2 seconds => keepalives
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(sendKeepAlives()));
    timer->start(2000);

    if(debug == "1")
        debugInitialisierung();
    //qDebug() << "SERVER STARTED";
}

void Server::newConnection()
{
    QTcpSocket * newSocket = tcpServer->nextPendingConnection();
    qDebug() << "NEW CONNECTION WITH: " << newSocket->peerAddress() << ":" << newSocket->peerPort();
    newUser(newSocket);
}

void Server::newUser(QTcpSocket *socket)
{
    quint32 uid = userIdCounter++;
    ChatSocket * chatSocket = new ChatSocket(socket, uid);
    connect(chatSocket,SIGNAL(newTcpData(DataElement,quint32)),SLOT(readData(DataElement,quint32)));
    users.createUser(chatSocket, uid);
}

void Server::readData(DataElement data, quint32 userId)
{
    if(userId != 0)
        users.user(userId)->socket()->resetTimeOutCounter();


    if(data.chatRoomIdentifier() != 0)
    {
        chatRooms->newData((ChatSocket*)sender(), data, userId);
        return;
    }

    switch(data.type())
    {
    case 1:
        if(data.subType() != 0)
            errorSubType();
        break;
    case 2:
        if(data.subType() ==0)
            readHandshake(data, userId);
        else
            errorSubType();
        break;
    default:
        errorType();
        break;
    }

}

void Server::readBroadCast(DataElement data, QHostAddress * peerAddress)
{
    if(data.type() == 0 && data.subType() == 0 && data.chatRoomIdentifier() == 0)
    {
        //read client port
        quint16 clientPort = data.readInt16();

        //send tcp port, and channellist to user
        DataElement newDataElement(0,0,1);
        QMap<quint32, QPair<QString, quint32> > informations = chatRooms->chatRoomsInfo();
        newDataElement.writeInt16(port);
        newDataElement.writeInt32(informations.count());
        foreach(quint32 id, informations.keys())
        {
            QPair<QString, quint32> pair = informations[id];
            newDataElement.writeInt32(id);
            newDataElement.writeString(pair.first);
            newDataElement.writeInt32(pair.second);
        }
        QUdpSocket * udpSocket = new QUdpSocket();
        udpSocket->writeDatagram(newDataElement.data(), *peerAddress, clientPort);
       //qDebug() << "UNICAST DATAGRAM WRITTEN";
    } else {
        qDebug() << "Unknown broadcast";
    }
}

void Server::sendKeepAlives()
{
    QList<AbstractUser*> currentUsers = users.allUsers();
    foreach(AbstractUser* abstractUser, currentUsers)
    {
        User* user = (User*)abstractUser;
        if(user->socket()->timeOutCounter() == 5)
        {
            users.remove(user->uid());
        }
        else
        {
            user->socket()->incrementTimeOutCounter();
            user->socket()->send(DataElement(0,1,0));
        }
    }
}

void Server::errorSubType()
{
    qDebug() << "Unknown SubType";
}

void Server::errorType()
{
    qDebug() << "Unknown Type";
}

void Server::readHandshake(DataElement data, quint32 userId)
{
    (void)data;
    //user modules will be ignored
    DataElement newDataElement(0,2,0);
    newDataElement.writeInt32(userId);
    //empty modules list
    newDataElement.writeInt32(0);
    users.user(userId)->socket()->send(newDataElement);
}

void Server::debugInitialisierung()
{
}

void Server::createChatRoom(QString name)
{
    chatRooms->addChatRoom(name);
}
