#include "users.h"

Users::Users()
{
}

void Users::createUser(ChatSocket *socket, quint32 uid)
{
    users[uid] =  (AbstractUser*)new User(uid, socket);
    uids[socket] = uid;
}

User* Users::user(ChatSocket *socket)
{
    return (User*)users[uids[socket]];
}

User* Users::user(quint32 uid)
{
    return  (User*)AbstractUsers::user(uid);
}

void Users::remove(quint32 uid)
{
    User * currentUser = user(uid);
    uids.remove(currentUser->socket());
    users.remove(uid);
    delete currentUser;
}

