#include "client.h"
#include "dataelement.h"
#include <QUdpSocket>
#include <QDebug>
#include "mainwindow.h"
#include <QTimer>
#include "dataelementviewer.h"

Client::Client(QString userName, quint16 serverPort, QObject *parent) :
    QObject(parent), userName(userName), server(0), serverPort(serverPort)
{
    broadCastSocket = new QUdpSocket;
    broadCastSocket->bind();
    //qDebug() << "client udp port" << broadCastSocket->localPort();
    udpSocket = new ChatSocket(broadCastSocket,0);
    connect(udpSocket,SIGNAL(newUdpData(DataElement,QHostAddress*,quint16)),SLOT(readUniCast(DataElement,QHostAddress*,quint16)));
    sendBroadCast();

    mainWindow = new MainWindow(this);
    mainWindow->show();
    connect(mainWindow,SIGNAL(chatRoomSelected(quint32)),SLOT(enterChatRoom(quint32)));
    connect(mainWindow,SIGNAL(createChatRoom(QString)),SLOT(createChatRoom(QString)));

    //every 2 seconds => keepalives
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), &chatRooms, SLOT(sendKeepAlives()));
    timer->start(2000);

    connect(mainWindow, SIGNAL(addIp(QHostAddress)), SLOT(addIp(QHostAddress)));
}

void Client::readUniCast(DataElement data, QHostAddress *address, quint16 port)
{
    DataElementViewer::getInstance()->addMessage(DataElementViewer::Client,
                                                 DataElementViewer::In,
                                                 DataElementViewer::UdpUnicast,
                                                 *address,
                                                 &data);
    (void*)address;
    (void)port;
    //readServerinformations
    if(data.type() == 0 && data.chatRoomIdentifier() == 0)
    {
        if(data.subType() == 1) {
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
        } else if(data.subType() == 2) {
            //do nothing
        }
    } else {
        qDebug() << data;
        qDebug() << "Unknown DataElement";
    }
}

void Client::sendBroadCast()
{
    DataElement data(0,0,0,0,0);
    DataElementViewer::getInstance()->addMessage(DataElementViewer::Client,
                                                 DataElementViewer::Out,
                                                 DataElementViewer::UdpBroadcast,
                                                 QHostAddress::Broadcast,
                                                 &data);
    broadCastSocket->writeDatagram(data.data(), QHostAddress::Broadcast, 10222);
    QUdpSocket socket;
    foreach (QHostAddress address, addresses)
    {
        socket.writeDatagram(data.data(), address, 10222);
        DataElementViewer::getInstance()->addMessage(DataElementViewer::Client,
                                                     DataElementViewer::Out,
                                                     DataElementViewer::UdpUnicast,
                                                     address,
                                                     &data);
    }
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
        connect(socket,SIGNAL(newTcpData(DataElement,quint32,QHostAddress)),SLOT(readTcpData(DataElement,quint32,QHostAddress)));
        DataElement data(0,2,0,0,0);
        data.writeInt32(0);
        socket->send(data, false);
        joinQueues[ip].append(id);
    } else {
        sendJoinRequest(socket, id);
    }
}

void Client::readTcpData(DataElement data, quint32 uid, QHostAddress address)
{
    DataElementViewer::getInstance()->addMessage(DataElementViewer::Client,
                                                 DataElementViewer::In,
                                                 DataElementViewer::Tcp,
                                                 address,
                                                 &data);

    qDebug() << "Client::readTcpData " << data;
    ChatSocket* socket = (ChatSocket*)sender();
    socket->resetTimeOutCounter();
    switch(data.type())
    {
    case 2:
        if(data.subType() == 1)
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
        if(data.subType() == 1)
        {
            showChatRoom(socket, data, uid);
        } else if(data.subType() <=5)
        {
            qDebug() << "ChatRoom access denied: " << data.readString();
        }
        break;
    case 4:
    case 6:
        chatRooms.newData(data, uid);
        break;
    default:
        qDebug() << "Client::newData unknown type" << data.type();
    }
}

void Client::sendJoinRequest(ChatSocket *socket, quint32 id)
{
    DataElement data(id, 3, 0, socket->userId(), 0);
    data.writeString(userName);
    data.writeString("Ich will rein");
    socket->send(data, false);
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
        server = new Server(serverPort);
    }
    server->createChatRoom(name);
    sendBroadCast();
}

void Client::addIp(QHostAddress address)
{
    addresses += address;
    sendBroadCast();
}
