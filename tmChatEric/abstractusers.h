#ifndef ABSTRACTUSERS_H
#define ABSTRACTUSERS_H

#include <QHash>
#include "abstractuser.h"

class AbstractUsers
{
public:
    AbstractUsers();
    bool contains(quint32 uid);
    quint32 length();
    AbstractUser* user(quint32 uid);
    QList<AbstractUser*> allUsers();

protected:
    QHash<quint32, AbstractUser*> users;
};

#endif // ABSTRACTUSERS_H
