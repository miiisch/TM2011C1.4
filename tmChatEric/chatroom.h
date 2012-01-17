#ifndef CHATROOM_H
#define CHATROOM_H

#include <QString>
#include "chatroomusers.h"
#include "abstractchatroom.h"

class DataElement;

class ChatRoom : public AbstractChatRoom
{
public:
    ChatRoom(quint32 id, QString name);
    quint32 chatRoomUsersCount();
    void newData(ChatSocket* socket, DataElement data, quint32 userId);

signals:

public slots:

private:
    void readJoinRequest(ChatSocket* socket, DataElement data, quint32 uid);
    void readChatMessage(DataElement data, quint32 uid);
    void sendChatMessage(DataElement data);
    void sendPrivateMessage(DataElement data);
    void readStatusMessage(DataElement data, quint32 uid);
    ChatRoomUsers users;
};

#endif // CHATROOM_H
