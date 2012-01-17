#ifndef USERS_H
#define USERS_H

#include <QHash>
#include "user.h"
#include "abstractusers.h"

class Users : public AbstractUsers
{
public:
    Users();
    void createUser(ChatSocket * socket, quint32 uid);
    User* user(ChatSocket * socket);
    User* user(quint32 uid);
    void remove(quint32 uid);

private:

    QHash<ChatSocket*,quint32> uids;
};

#endif // USERS_H
