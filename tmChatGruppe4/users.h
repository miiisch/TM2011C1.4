#ifndef USERS_H
#define USERS_H

#include <QHash>
#include "user.h"

class Users
{
public:
    Users();
    void createUser(ChatSocket * socket, quint32 uid);
    User* user(ChatSocket * socket);
    User* user(quint32 uid);
    void remove(quint32 uid);
    bool contains(quint32 uid);
    quint32 length();
    QList<User*> allUsers();

private:

    QHash<quint32, User*> users;
    QHash<ChatSocket*,quint32> uids;
};

#endif // USERS_H
