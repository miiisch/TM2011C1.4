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

Server::Server(quint16 serverPort, bool enableKeepalives, bool denyAll, bool gui, QObject *parent) :
    QObject(parent), tcpServer(new QTcpServer()), userIdCounter(1), _sendKeepalives(enableKeepalives), _denyAll(denyAll), _gui(gui)
{
    chatRooms = new ChatRooms();
    QUdpSocket * socket = new QUdpSocket;
    qDebug() << socket->bind(10222);
    udpChatSocket = new ChatSocket(socket);
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

void Server::createChatRoom(QString name)
{
    chatRooms->addChatRoom(name, _denyAll);
}

void Server::activateKeepalives(bool x)
{
    _sendKeepalives = x;
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

void Server::newCommand(QString command)
{
    if (command.startsWith("add ip "))
    {
        QString address = right(command, "add ip ");
        QHostAddress add(address);
        emit addIp(add);
        writeStatus("IP Address " + add.toString() + " added", 5000);
    }
    else if (command.startsWith("set null "))
    {
        QString next = right(command, "set null ");
        bool client = true;
        bool server = true;
        if (next.startsWith("client "))
        {
            next = right(next, "client ");
            server = false;
        }
        else if (next.startsWith("server "))
        {
            next = right(next, "server ");
            client = false;
        }

        if (next != "0" && next != "1") {
            writeStatus("Unknown command: " + command, 5000);
            return;
        }

        bool enable = (next == "1");

        if (client)
            emit enableClientKeepalive(enable);
        if (server)
            emit enableServerKeepalive(enable);

        QString message = "Keepalives ";
        message += enable ? "enabled " : "disabled ";
        message += "for ";
        if (client && server)
            message += "Client and Server";
        else
            message += client ? "Client" : "Server";

        writeStatus(message, 5000);
    }
    else if (command.startsWith("set deny "))
    {
        QString next = right(command, "set deny ");
        QString message = "Deny everything %1";
        if (next == "1")
        {
            emit enableDenyAll(true);
            writeStatus(message.arg("enabled"));
        }
        else if (next == "0")
        {
            emit enableDenyAll(false);
            writeStatus(message.arg("disabled"));
        }
        else
        {
            writeStatus("Unknown command: " + command, 5000);
            return;
        }
    }
    else if (command.startsWith("create "))
    {
        QString next = right(command, "create ");
        emit createChatRoom(next);
    }
    else if (command.startsWith("close "))
    {
        quint32 id;
        QString next = right(command, "close ");
        bool valid = splitInt(next, id);
        if (!valid)
        {
            writeStatus("Unknown command: " + command, 5000);
            return;
        }
        emit closeChannel(id, next);
    }
    else
        writeStatus("Unknown command: " + command, 5000);
}

void Server::writeStatus(QString text, int timeOut)
{
    if (_gui)
        emit writeToStatusbar(text, timeOut);
    else
        printf(text);
}

QString Server::right(QString &input, const char cutoffLeft[])
{
    return input.right(input.length() - QString(cutoffLeft).length());
}

bool Server::splitInt(QString &s, quint32 &i)
{
    QList<QString> split = s.split(" ");
    if (split.isEmpty())
        return false;

    bool ok;
    i = split[0].toInt(&ok);
    if (!ok) {
        return false;
    }

    if (split.size() > 0)
        s = s.right(s.length() - split[0].size() - (split.size() > 1 ? 1 : 0));

    return true;
}
