#ifndef CHATROOMUSERS_H
#define CHATROOMUSERS_H

#include <QHash>
#include "chatroomuser.h"

class ChatRoomUsers
{
public:
    ChatRoomUsers();
    quint32 length();
    ChatRoomUser* user(quint32 uid);
    void addUser(ChatSocket * socket, quint32 uid, QString userName, ChatRoomUser::Status status, bool moderatorPermission, bool kickPermission);
    QList<ChatRoomUser*> allUsers();
    void remove(quint32 uid);
    bool contains(quint32 uid);

private:
    QHash<quint32, ChatRoomUser*> users;
};

#endif // CHATROOMUSERS_H
