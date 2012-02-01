#include "server.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include "chatsocket.h"
#include <QDebug>
#include "dataelement.h"
#include <QTimer>
#include "dataelementviewer.h"
#include <cstdio>
#include <QCoreApplication>

Server::Server(quint16 serverPort, bool enableSendKeepalives, bool enableCheckKeepalives, bool denyAll, bool gui, QObject *parent) :
    QObject(parent), tcpServer(new QTcpServer()), userIdCounter(1), _sendKeepalives(enableSendKeepalives), _checkKeepalives(enableCheckKeepalives), _denyAll(denyAll), _gui(gui)
{
    chatRooms = new ChatRooms();
    QUdpSocket * socket = new QUdpSocket;
    if(!socket->bind(10222))
        qWarning("Can't bind to UDP port 10222, so discovery of chat servers will not work.");
    udpChatSocket = new ChatSocket(socket, gui);
    connect(udpChatSocket,SIGNAL(newUdpData(DataElement,QHostAddress*,quint16,QUdpSocket*)),SLOT(readBroadCast(DataElement,QHostAddress*,quint16,QUdpSocket*)));
    tcpServer->listen(QHostAddress::Any, serverPort);
    tcpPort = tcpServer->serverPort();
    connect(tcpServer,SIGNAL(newConnection()),SLOT(newConnection()));

    //every 2 seconds => keepalives
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(sendKeepAlives()));
    timer->start(2000);
}

void Server::newConnection()
{
    QTcpSocket * newSocket = tcpServer->nextPendingConnection();
    newUser(newSocket);
}

void Server::newUser(QTcpSocket *socket)
{
    quint32 uid = userIdCounter++;
    ChatSocket * chatSocket = new ChatSocket(socket, uid, _gui);
    connect(chatSocket,SIGNAL(newTcpData(DataElement,quint32,QHostAddress)),SLOT(readData(DataElement,quint32,QHostAddress)));
    users.createUser(chatSocket, uid);
}

void Server::readData(DataElement data, quint32 userId, QHostAddress address)
{
    if(_gui)
    {
        DataElementViewer::getInstance()->addMessage(DataElementViewer::Server,
                                                     DataElementViewer::In,
                                                     DataElementViewer::Tcp,
                                                     address,
                                                     &data);
    }

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
    if(_gui)
    {
        DataElementViewer::getInstance()->addMessage(DataElementViewer::Server,
                                                     DataElementViewer::In,
                                                     DataElementViewer::UdpBroadcast,
                                                     *peerAddress,
                                                     &data);
    }

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
        if(_gui)
        {
            DataElementViewer::getInstance()->addMessage(DataElementViewer::Server,
                                                         DataElementViewer::Out,
                                                         DataElementViewer::UdpUnicast,
                                                         *peerAddress,
                                                         &newDataElement);
        }
    } else {
        qDebug() << "Unknown broadcast";
    }
}

void Server::sendKeepAlives()
{
    QList<User*> currentUsers = users.allUsers();
    foreach(User* user, currentUsers)
    {
        if(user->socket()->timeOutCounter() == 5 && _checkKeepalives)
        {
            chatRooms->userConnectionLost(user->uid());
            users.remove(user->uid());
        }
        else if(user->socket())
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
    int count = data.readInt32();
    QList<QString> * userModulesList = &(users.user(userId)->modules);
    for (int i = 0; i < count; ++i)
        *userModulesList << data.readString();

    DataElement newDataElement(0,2,1,0,0);
    newDataElement.writeInt32(userId);
    //empty modules list
    newDataElement.writeInt32(0);
    users.user(userId)->socket()->send(newDataElement, true);
}

void Server::createChatRoom(QString name)
{
    chatRooms->addChatRoom(name, _denyAll);
}

void Server::activateSendKeepalives(bool x)
{
    _sendKeepalives = x;
}

void Server::activateCheckKeepalives(bool x)
{
    _checkKeepalives = x;
}

void Server::registerLocalClient(quint32 clientId)
{
    chatRooms->registerLocalClient(clientId);
}

void Server::activateDenyAll(bool denyAll)
{
    _denyAll = denyAll;
    chatRooms->setDenyAll(denyAll);
}

void Server::closeChatRoom(quint32 id, QString text)
{
    chatRooms->removeChatRoom(id, text);
}

