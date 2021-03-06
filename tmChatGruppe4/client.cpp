#include "client.h"
#include "chatsocket.h"
#include "dataelement.h"
#include <QUdpSocket>
#include <QDebug>
#include "mainwindow.h"
#include <QTimer>
#include "dataelementviewer.h"

Client::Client(QString userName, quint16 serverPort) :
    QObject(0), userName(userName), server(0), serverPort(serverPort), _serverSendKeepalives(true), _serverCheckKeepalives(true), _serverDenyAll(false)
{
    broadCastSocket = new QUdpSocket;
    broadCastSocket->bind(serverPort);
    //qDebug() << "client udp port" << broadCastSocket->localPort();
    udpSocket = new ChatSocket(broadCastSocket, true);
    connect(udpSocket, SIGNAL(newUdpData(DataElement,QHostAddress*,quint16,QUdpSocket*)), SLOT(readUniCast(DataElement,QHostAddress*,quint16)));

    mainWindow = new MainWindow(this);
    mainWindow->show();
    connect(mainWindow, SIGNAL(processCommand(QString)), SIGNAL(processCommand(QString)));
    connect(mainWindow, SIGNAL(chatRoomSelected(quint32)), SLOT(enterChatRoom(quint32)));
    connect(mainWindow, SIGNAL(createChatRoom(QString)), SLOT(createChatRoom(QString)));

    sendBroadCast();

    //every 2 seconds => keepalives
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), &chatRooms, SLOT(sendKeepAlives()));
    timer->start(2000);
}

void Client::readUniCast(DataElement data, QHostAddress *address, quint16 port)
{
    DataElementViewer::getInstance()->addMessage(DataElementViewer::Client,
                                                 DataElementViewer::In,
                                                 DataElementViewer::UdpUnicast,
                                                 *address,
                                                 &data);
    (void)port;
    //readServerinformations
    if(data.type() == 0 && data.chatRoomIdentifier() == 0)
    {
        if(data.subType() == 1) {
            quint16 tcpPort = data.readInt16();
            quint32 listLength = data.readInt32();
            for(int i = 0; i < (int)listLength; ++i)
            {
                quint32 id = data.readInt32();
                QString name = data.readString();
                quint32 numberOfUsers = data.readInt32();
                chatRoomInfo << new ChatRoomInfo(tcpPort, id, name, numberOfUsers, *address, udpSocket->localIp() == *address);
            }
            mainWindow->setChatRoomInfo(chatRoomInfo);
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
    foreach (QHostAddress address, addresses)
    {
        broadCastSocket->writeDatagram(data.data(), address, 10222);
        DataElementViewer::getInstance()->addMessage(DataElementViewer::Client,
                                                     DataElementViewer::Out,
                                                     DataElementViewer::UdpUnicast,
                                                     address,
                                                     &data);
    }
    mainWindow->clearChatRoomInfo();
    chatRoomInfo.clear();
}

void Client::enterChatRoom(quint32 row)
{
    ChatRoomInfo * info = chatRoomInfo[row];
    QHostAddress ip = info->address;
    if (chatRooms.containsRoom(ip, info->id))
        return;
    quint16 port = info->tcpPort;
    ChatSocket* socket = chatRooms.serverConnection(ip, port);
    if(socket->userId() == 0)
    {
        if (!socket->handShakeDone())
        {
            // request user id with handshake
            connect(socket,SIGNAL(newTcpData(DataElement,quint32,QHostAddress)),SLOT(readTcpData(DataElement,quint32,QHostAddress)));
            DataElement data(0,2,0,0,0);
            data.writeInt32(0);
            socket->send(data, false);
            socket->setHandShakeDone();
        }
        joinQueues[ip].append(info);
    } else {
        sendJoinRequest(socket, info);
    }
}

void Client::readTcpData(DataElement data, quint32 uid, QHostAddress address)
{
    DataElementViewer::getInstance()->addMessage(DataElementViewer::Client,
                                                 DataElementViewer::In,
                                                 DataElementViewer::Tcp,
                                                 address,
                                                 &data);

    ChatSocket* socket = (ChatSocket*)sender();
    socket->resetTimeOutCounter();
    switch(data.type())
    {
    case 1:
        break;
    case 2:
        if(data.subType() == 1)
        {
            quint32 id = data.readInt32();
            socket->setUserId(id);
            if(socket->ip() == socket->localIp() && server != 0)
            {
                server->registerLocalClient(id);
            }
            foreach(ChatRoomInfo* info, joinQueues[socket->ip()]) {
                sendJoinRequest(socket, info);
            }
            joinQueues.clear();
        } else {
            qDebug() << "Unknown subType";
        }
        break;
    case 3:
        if(data.subType() == 1)
        {
            activateChatRoom(socket, data, uid);
        } else if(data.subType() <=5)
        {
            chatRooms.denyJoin(address, data.chatRoomIdentifier(), data);
        }
        break;
    case 4:
    case 6:
    case 7:
    case 9:
    case 10:
        chatRooms.newData(data, socket->ip(), uid);
        break;
    default:
        qDebug() << "Client::newData unknown type" << data.type();
    }
}

void Client::sendJoinRequest(ChatSocket *socket, ChatRoomInfo *info)
{
    DataElement data(info->id, 3, 0, socket->userId(), 0);
    data.writeString(userName);
    data.writeString("Ich will rein");
    socket->send(data, false);
    chatRooms.addChatRoom(socket, info->id, socket->userId(), info->name);
}

void Client::activateChatRoom(ChatSocket * socket, DataElement data, quint32)
{
    quint32 id = data.chatRoomIdentifier();
    quint32 listLength = data.readInt32();
    QMap<quint32, UserInfo> userInfo;
    for(int i = 0; i < (int)listLength; ++i)
    {
        quint32 id = data.readInt32();
        quint32 status = data.readInt32();
        QString name = data.readString();
        quint32 numberOfModules = data.readInt32();
        (void)numberOfModules;
        userInfo[id] = UserInfo(id, name, (Status)status);
    }
    chatRooms.activateChatRoom(socket->ip(), id, userInfo);
}

void Client::createChatRoom(QString name)
{
    if(server == 0)
    {
        server = new Server(serverPort, _serverSendKeepalives, _serverCheckKeepalives, _serverDenyAll, true, this);
        emit serverCreated(server);
    }
    server->createChatRoom(name);
    sendBroadCast();
}

void Client::addIp(QHostAddress address)
{
    if(!addresses.contains(address)) {
        addresses += address;
        sendBroadCast();
    }
}

void Client::enableSendKeepalivesServer(bool activate)
{
    _serverSendKeepalives = activate;
    if (server != 0)
        server->activateSendKeepalives(activate);
}

void Client::enableCheckKeepalivesServer(bool activate)
{
    _serverCheckKeepalives = activate;
    if (server != 0)
        server->activateCheckKeepalives(activate);
}

void Client::enableSendKeepalivesClient(bool activate)
{
    chatRooms.activateSendKeepalives(activate);
}

void Client::enableCheckKeepalivesClient(bool activate)
{
    chatRooms.activateCheckKeepalives(activate);
}

void Client::denyAllServer(bool denyAll)
{
    _serverDenyAll = denyAll;
    if (server != 0)
        server->activateDenyAll(denyAll);
}

void Client::closeChatRoom(quint32 id, QString message)
{
    if (server != 0)
        server->closeChatRoom(id, message);
    sendBroadCast();
}

void Client::showCommandLineStatus(QString text)
{
    mainWindow->showCommandLineStatus(text);
}
