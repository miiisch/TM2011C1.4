#ifndef CLIENTCHATROOM_H
#define CLIENTCHATROOM_H

#include "chatroomwindow.h"
#include "types.h"
#include "chatsocket.h"

class ClientChatRoom : public QObject
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
    QString name();

public slots:
    void sendMessage(QString text);
    void showChatMessage(DataElement data, quint32 userId);
    void showDenyMessage(DataElement data, quint32 userId);
    void sendUserQuit();

signals:
    void closed(QHostAddress address, quint32 id);

private:
    quint32 _id;
    QString _name;
   void  readStatusMessage(DataElement data);
    ChatSocket * _socket;
    quint32 _userId;
    ChatRoomWindow * window;
    QList<UserInfo> userInfo;
};

#endif // CLIENTCHATROOM_H
