#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include "chatsocket.h"
#include "clientchatrooms.h"
#include "server.h"

class MainWindow;

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QString userName, quint16 serverPort, QObject *parent = 0);

signals:

public slots:
    void readUniCast(DataElement data, QHostAddress * address, quint16 port);
    void readTcpData(DataElement data, quint32 uid, QHostAddress address);
    void enterChatRoom(quint32);
    void sendJoinRequest(ChatSocket *socket, ChatRoomInfo *info);
    void activateChatRoom(ChatSocket* socket, DataElement data, quint32 uid);
    void createChatRoom(QString name);
    void sendBroadCast();

private slots:
    void addIp(QHostAddress);

private:
    ChatSocket * udpSocket;
    QUdpSocket * broadCastSocket;
    MainWindow * mainWindow;
    QString userName;
//    QHash<QHostAddress*, QHash<quint32, ChatRoomInfo*> > chatRoomInfo;
    QList<ChatRoomInfo*> chatRoomInfo;
    ClientChatRooms chatRooms;
    QHash<QHostAddress,QList<ChatRoomInfo*> > joinQueues;
    Server * server;

    QList<QHostAddress> addresses;
    quint16 serverPort;
};

#endif // CLIENT_H
