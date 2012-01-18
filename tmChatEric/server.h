#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QHostAddress>
#include "chatrooms.h"
#include "users.h"

typedef QPair<QString, quint32> Pair;

class DataElement;
class QTcpServer;
class QUdpSocket;
class ChatSocket;
class QTcpSocket;

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QString debug, QObject *parent = 0);
    void createChatRoom(QString name);

signals:

public slots:
    void newConnection();
    void readData(DataElement data, quint32 userId, QHostAddress address);
    void sendKeepAlives();
    void readBroadCast(DataElement data, QHostAddress * peerAddress, quint16 port);

private:

    void newUser(QTcpSocket * socket);
    void readHandshake(DataElement data, quint32 userId);
    void errorType();
    void errorSubType();
    void errorSender();
    QTcpServer * tcpServer;
    ChatSocket * udpChatSocket;
    quint16 tcpPort;
    ChatRooms * chatRooms;
    int userIdCounter;
    Users users;



    void debugInitialisierung();
};

#endif // SERVER_H
