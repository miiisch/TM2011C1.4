#ifndef CHATROOM_H
#define CHATROOM_H

#include <QString>
#include "chatroomusers.h"

class DataElement;

class ChatRoom
{
public:
    ChatRoom(quint32 id, QString name);
    quint32 chatRoomUsersCount();
    void newData(ChatSocket* socket, DataElement data, quint32 userId);
    void userConnectionLost(quint32 uid);
    quint32 id();
    QString name();

signals:

public slots:

private:
    void readJoinRequest(ChatSocket* socket, DataElement data, quint32 uid);
    void readChatMessage(DataElement data, quint32 uid);
    void sendChatMessage(DataElement data);
    void sendPrivateMessage(DataElement data);
    void readStatusMessage(DataElement data, quint32 uid);
    void readActionMessage(DataElement data, quint32 uid);
    ChatRoomUsers users;

    quint32 _id;
    QString _name;
};

#endif // CHATROOM_H
