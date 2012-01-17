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
    explicit Client(QString userName, QObject *parent = 0);

signals:

public slots:
    void readUniCast(DataElement data, QHostAddress * address);
    void readTcpData(DataElement data, quint32 uid);
    void enterChatRoom(quint32);
    void sendJoinRequest(ChatSocket* socket, quint32 id);
    void showChatRoom(ChatSocket* socket, DataElement data, quint32 uid);
    void createChatRoom(QString name);

private:
    void sendBroadCast();
    ChatSocket * udpSocket;
    MainWindow * mainWindow;
    QString userName;
    QHash<quint32, ChatRoomInfo*> chatRoomInfo;
    ClientChatRooms chatRooms;
    QHash<QHostAddress,QList<quint32> > joinQueues;
    Server * server;
};

#endif // CLIENT_H
