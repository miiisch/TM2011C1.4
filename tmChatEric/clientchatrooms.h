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
    void activateChatRoom(QHostAddress address, quint32 id, QList<UserInfo> userInfo);
    void denyJoin(QHostAddress address, quint32 id, DataElement & data);
    bool containsRoom(QHostAddress address, quint32 id);

public slots:
    void sendKeepAlives();

private:
    QHash<QHostAddress, QHash<quint32, ClientChatRoom*> > chatRooms;
};

#endif // CLIENTCHATROOMS_H
