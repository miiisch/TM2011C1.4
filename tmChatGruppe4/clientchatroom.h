#ifndef CLIENTCHATROOM_H
#define CLIENTCHATROOM_H

#include "chatroomwindow.h"
#include "types.h"
#include "dataelement.h"

class ChatSocket;

class ClientChatRoom : public QObject
{
    Q_OBJECT
public:
    ClientChatRoom(ChatSocket* socket, quint32 id, QString name, quint32 userId);
    void newData(DataElement data, quint32 userId);
    ChatSocket * socket();
    quint32 userId();
    quint32 id();
    void activate(QMap<quint32, UserInfo> userInfo);
    void denyJoin(DataElement & data);
    void serverQuit();
    QString name();

public slots:
    void sendMessage(QString text);
    void showChatMessage(DataElement data, quint32 userId);
    void showDenyMessage(DataElement data, quint32 userId);
    void sendUserQuit(bool joinNotDenied, QString reason);

signals:
    void closed(QHostAddress address, quint32 id);

private:
    quint32 _id;
    QString _name;
    void readStatusMessage(DataElement data);
    void readActionAceptedMessage(DataElement data);
    void readActionDeniedMessage(DataElement data);
    void disableChatroom(QString reason);
    ChatSocket * _socket;
    quint32 _userId;
    ChatRoomWindow * window;
    QMap<quint32, UserInfo> userInfo;
    bool splitInt(QString &s, quint32 &i);
};

#endif // CLIENTCHATROOM_H
