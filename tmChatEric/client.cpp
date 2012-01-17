#include "client.h"
#include "dataelement.h"
#include <QUdpSocket>
#include <QDebug>
#include "mainwindow.h"
#include <QTimer>

Client::Client(QString userName, QObject *parent) :
    QObject(parent), userName(userName)
{
    QUdpSocket * socket = new QUdpSocket;
    socket->bind(12245);
    udpSocket = new ChatSocket(socket,0);
    connect(udpSocket,SIGNAL(newUdpData(DataElement,QHostAddress*)),SLOT(readUniCast(DataElement,QHostAddress*)));
    sendBroadCast();

    mainWindow = new MainWindow;
    mainWindow->show();
    connect(mainWindow,SIGNAL(chatRoomSelected(quint32)),SLOT(enterChatRoom(quint32)));
    connect(mainWindow,SIGNAL(createChatRoom(QString)),SLOT(createChatRoom(QString)));

    //every 2 seconds => keepalives
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), &chatRooms, SLOT(sendKeepAlives()));
    timer->start(2000);
}

void Client::readUniCast(DataElement data, QHostAddress *address)
{
    (void*)address;
    //readServerinformations
    if(data.type() == 0 && data.chatRoomIdentifier() == 0 && data.subType() == 1)
    {
        quint16 tcpPort = data.readInt16();
        quint32 listLength = data.readInt32();
        QList<ChatRoomInfo*> chatRoomsInfo;
        for(int i=0;i<listLength;++i)
        {
            quint32 id = data.readInt32();
            QString name = data.readString();
            quint32 numberOfUsers = data.readInt32();
            ChatRoomInfo * info = new ChatRoomInfo(tcpPort, id, name, numberOfUsers, *address);
            chatRoomsInfo << info;
            chatRoomInfo[id] = info;
        }
        mainWindow->setChatRoomInfo(chatRoomsInfo);
    } else {
        qDebug() << "Unknown DataElement";
    }
}

void Client::sendBroadCast()
{
    QUdpSocket * socket = new QUdpSocket();
    DataElement data(0,0,0);
    data.writeInt16(12245);
    socket->writeDatagram(data.data(),QHostAddress::Broadcast,13167);
    //qDebug() << "BROADCAST DATAGRAM WRITTEN!";
}

void Client::enterChatRoom(quint32 id)
{
    ChatRoomInfo * info = chatRoomInfo[id];
    QHostAddress ip = info->address;
    quint16 port = info->tcpPort;
    ChatSocket* socket = chatRooms.serverConnection(ip, port);
    if(socket->userId() == 0)
    {
        // request user id with handshake
        connect(socket,SIGNAL(newTcpData(DataElement,quint32)),SLOT(readTcpData(DataElement,quint32)));
        DataElement data(0,2,0);
        data.writeInt32(0);
        socket->send(data);
        joinQueues[ip].append(id);
    } else {
        sendJoinRequest(socket, id);
    }
}

void Client::readTcpData(DataElement data, quint32 uid)
{
    ChatSocket* socket = (ChatSocket*)sender();
    socket->resetTimeOutCounter();
    switch(data.type())
    {
    case 2:
        if(data.subType() == 0)
        {
            foreach(quint32 id, joinQueues[socket->ip()]) {
                socket->setUserId(data.readInt32());
                sendJoinRequest(socket, id);
            }
        } else {
            qDebug() << "Unknown subType";
        }
        break;
    case 3:
        if(data.subType() == 0)
        {
            showChatRoom(socket, data, uid);
        } else if(data.subType() <=5)
        {
            qDebug() << "ChatRoom access denied: " << data.readString();
        }
        break;
    case 4:
    case 5:
        chatRooms.newData(data, uid);
        break;

    }
}

void Client::sendJoinRequest(ChatSocket *socket, quint32 id)
{
    DataElement data(id, 3, 0);
    data.writeString(userName);
    data.writeString("Ich will rein");
    socket->send(data);
}

void Client::showChatRoom(ChatSocket * socket, DataElement data, quint32 uid)
{
    quint32 id = data.chatRoomIdentifier();
    quint32 listLength = data.readInt32();
    QList<UserInfo> userInfo;
    for(int i=0;i<listLength;++i)
    {
        quint32 id = data.readInt32();
        quint32 status = data.readInt32();
        QString name = data.readString();
        quint32 numberOfModules = data.readInt32();
        (void)numberOfModules;
        userInfo.append(UserInfo(id, name, (Status)status));
    }
    chatRooms.addChatRoom(socket, id, uid, chatRoomInfo[id]->name, userInfo);
}

void Client::createChatRoom(QString name)
{
    if(server == 0)
    {
        server = new Server(0);
    }
    server->createChatRoom(name);
    sendBroadCast();
}
