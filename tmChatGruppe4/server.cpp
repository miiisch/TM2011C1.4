#include "server.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include "chatsocket.h"
#include <QDebug>
#include "dataelement.h"
#include <QTimer>
#include "dataelementviewer.h"

Server::Server(quint16 serverPort, bool enableKeepalives, QObject *parent) :
    QObject(parent), tcpServer(new QTcpServer()), userIdCounter(1), _sendKeepalives(enableKeepalives)
{
    chatRooms = new ChatRooms();
    QUdpSocket * socket = new QUdpSocket;
    qDebug() << socket->bind(10222);
    udpChatSocket = new ChatSocket(socket);
    connect(udpChatSocket,SIGNAL(newUdpData(DataElement,QHostAddress*,quint16,QUdpSocket*)),SLOT(readBroadCast(DataElement,QHostAddress*,quint16,QUdpSocket*)));
    tcpServer->listen(QHostAddress::Any, serverPort);
    tcpPort = tcpServer->serverPort();
    //qDebug() << " serverport " << tcpPort;
    connect(tcpServer,SIGNAL(newConnection()),SLOT(newConnection()));

    //every 2 seconds => keepalives
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(sendKeepAlives()));
    timer->start(2000);

//    if(debug == "1")
//        debugInitialisierung();
    //qDebug() << "SERVER STARTED";
}

void Server::newConnection()
{
    QTcpSocket * newSocket = tcpServer->nextPendingConnection();
//    qDebug() << "NEW CONNECTION WITH: " << newSocket->peerAddress() << ":" << newSocket->peerPort();
    newUser(newSocket);
}

void Server::newUser(QTcpSocket *socket)
{
    quint32 uid = userIdCounter++;
    ChatSocket * chatSocket = new ChatSocket(socket, uid);
    connect(chatSocket,SIGNAL(newTcpData(DataElement,quint32,QHostAddress)),SLOT(readData(DataElement,quint32,QHostAddress)));
    users.createUser(chatSocket, uid);
}

void Server::readData(DataElement data, quint32 userId, QHostAddress address)
{
    DataElementViewer::getInstance()->addMessage(DataElementViewer::Server,
                                                 DataElementViewer::In,
                                                 DataElementViewer::Tcp,
                                                 address,
                                                 &data);
    //before the handshake is completed, the user can't know his id
    //sender = 0 in NULL Message
    if (data.type() != 2 && data.type() != 1 && userId != data.sender())
    {
        qDebug() << "server side" << "UserId: " << userId << " Sender: " << data.sender();
        errorSender();
        return;
    }

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
        if(data.subType() == 0)
            readHandshake(data, userId);
        else
            errorSubType();
        break;
    default:
        errorType();
        break;
    }

}

void Server::readBroadCast(DataElement data, QHostAddress * peerAddress, quint16 port, QUdpSocket* udpSocket)
{
    DataElementViewer::getInstance()->addMessage(DataElementViewer::Server,
                                                 DataElementViewer::In,
                                                 DataElementViewer::UdpBroadcast,
                                                 *peerAddress,
                                                 &data);
    if(data.type() == 0 && data.subType() == 0 && data.chatRoomIdentifier() == 0)
    {
        //send tcp port, and channellist to user
        DataElement newDataElement(0,0,1,0,0);
        QMap<quint32, QPair<QString, quint32> > informations = chatRooms->chatRoomsInfo();
        newDataElement.writeInt16(tcpPort);
        newDataElement.writeInt32(informations.count());
        foreach(quint32 id, informations.keys())
        {
            QPair<QString, quint32> pair = informations[id];
            newDataElement.writeInt32(id);
            newDataElement.writeString(pair.first);
            newDataElement.writeInt32(pair.second);
        }
        udpSocket->writeDatagram(newDataElement.data(), *peerAddress, port);
        DataElementViewer::getInstance()->addMessage(DataElementViewer::Server,
                                                     DataElementViewer::Out,
                                                     DataElementViewer::UdpUnicast,
                                                     *peerAddress,
                                                     &newDataElement);
    } else {
        qDebug() << "Unknown broadcast";
    }
}

void Server::sendKeepAlives()
{
    QList<User*> currentUsers = users.allUsers();
    foreach(User* user, currentUsers)
    {
        if(user->socket()->timeOutCounter() == 5)
        {
            chatRooms->userConnectionLost(user->uid());
            users.remove(user->uid());
        }
        else
        {
            user->socket()->incrementTimeOutCounter();
            if (_sendKeepalives)
                user->socket()->send(DataElement(0,1,0,0,0), true);
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

void Server::errorSender()
{
    qDebug() << "Sender not matching";
}

void Server::readHandshake(DataElement data, quint32 userId)
{
    (void)data;
    //user modules will be ignored
    DataElement newDataElement(0,2,1,0,0);
    newDataElement.writeInt32(userId);
    //empty modules list
    newDataElement.writeInt32(0);
    users.user(userId)->socket()->send(newDataElement, true);
}

void Server::debugInitialisierung()
{
}

void Server::createChatRoom(QString name)
{
    chatRooms->addChatRoom(name);
}

void Server::activateKeepalives(bool x)
{
    _sendKeepalives = x;
}