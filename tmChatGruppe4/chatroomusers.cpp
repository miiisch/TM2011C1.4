#include "chatroomusers.h"

ChatRoomUsers::ChatRoomUsers()
{
}

quint32 ChatRoomUsers::length()
{
    return users.count();
}

ChatRoomUser * ChatRoomUsers::user(quint32 uid)
{
    return (ChatRoomUser*) users[uid];
}

void ChatRoomUsers::addUser(ChatSocket* socket, quint32 uid, QString userName, ChatRoomUser::Status status, bool moderatorPermission, bool kickPermission)
{
    users[uid] = new ChatRoomUser(socket, uid, userName, status, moderatorPermission, kickPermission);
}

QList<ChatRoomUser *> ChatRoomUsers::allUsers()
{
    return users.values();
}

void ChatRoomUsers::remove(quint32 uid)
{
    ChatRoomUser* user = users[uid];
    users.remove(uid);
    delete user;
}

bool ChatRoomUsers::contains(quint32 uid)
{
    return users.contains(uid);
}
