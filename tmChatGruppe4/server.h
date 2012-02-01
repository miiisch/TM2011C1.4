#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QHostAddress>
#include <QSet>
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
    explicit Server(quint16 serverPort, bool enableSendKeepalives, bool enableCheckKeepalives, bool denyAll, bool gui = true, QObject *parent = 0);
    void createChatRoom(QString name);
    void activateSendKeepalives(bool);
    void activateCheckKeepalives(bool);
    void activateDenyAll(bool);
    void registerLocalClient(quint32 clientId);
    void closeChatRoom(quint32 id, QString text);

public slots:
    void newConnection();
    void readData(DataElement data, quint32 userId, QHostAddress address);
    void sendKeepAlives();
    void readBroadCast(DataElement data, QHostAddress * peerAddress, quint16 port, QUdpSocket* udpSocket);

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
    quint32 userIdCounter;
    Users users;
    bool _sendKeepalives;
    bool _checkKeepalives;
    bool _denyAll;
    bool _gui;
};

#endif // SERVER_H
