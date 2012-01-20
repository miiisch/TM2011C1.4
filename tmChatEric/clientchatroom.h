#ifndef CLIENTCHATROOM_H
#define CLIENTCHATROOM_H

#include "abstractchatroom.h"
#include "chatroomwindow.h"
#include "types.h"

class ClientChatRoom : public QObject, AbstractChatRoom
{
    Q_OBJECT
public:
    ClientChatRoom(ChatSocket* socket, quint32 id, QString name, quint32 userId);
    void newData(DataElement data, quint32 userId);
    ChatSocket * socket();
    quint32 userId();
    quint32 id();
    void activate(QList<UserInfo> userInfo);
    void denyJoin(DataElement & data);
    void serverQuit();

public slots:
    void sendMessage(QString text);
    void showChatMessage(DataElement data, quint32 userId);
    void showDenyMessage(DataElement data, quint32 userId);
    void sendUserQuit();

private:
   void  readStatusMessage(DataElement data);
    ChatSocket * _socket;
    quint32 _userId;
    ChatRoomWindow * window;
    QList<UserInfo> userInfo;
};

#endif // CLIENTCHATROOM_H
