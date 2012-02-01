#ifndef CLIENTCHATROOMS_H
#define CLIENTCHATROOMS_H

#include "chatsocket.h"
#include "clientchatroom.h"

class ClientChatRooms: public QObject
{
    Q_OBJECT
public:
    ClientChatRooms();

    void newData(DataElement data, QHostAddress address, quint32 userId);
    ChatSocket * serverConnection(QHostAddress ip, quint16 port);
    void addChatRoom(ChatSocket* socket, quint32 id, quint32 userId, QString name);
    void activateChatRoom(QHostAddress address, quint32 id, QMap<quint32, UserInfo> userInfo);
    void denyJoin(QHostAddress address, quint32 id, DataElement & data);
    bool containsRoom(QHostAddress address, quint32 id);
    void activateSendKeepalives(bool);
    void activateCheckKeepalives(bool);

public slots:
    void sendKeepAlives();

private slots:
    void roomClosed(QHostAddress address, quint32 id);

private:
    QHash<QHostAddress, QHash<quint32, ClientChatRoom*> > chatRooms;
    bool _sendKeepalives;
    bool _checkKeepalives;
};

#endif // CLIENTCHATROOMS_H
