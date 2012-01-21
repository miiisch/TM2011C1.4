#include "users.h"

Users::Users()
{
}

void Users::createUser(ChatSocket *socket, quint32 uid)
{
    users[uid] = new User(uid, socket);
    uids[socket] = uid;
}

User* Users::user(ChatSocket *socket)
{
    return users[uids[socket]];
}

User* Users::user(quint32 uid)
{
    return users[uid];
}

void Users::remove(quint32 uid)
{
    User * currentUser = user(uid);
    uids.remove(currentUser->socket());
    users.remove(uid);
    delete currentUser;
}

quint32 Users::length()
{
    return users.size();
}

QList<User *> Users::allUsers()
{
    return users.values();
}

