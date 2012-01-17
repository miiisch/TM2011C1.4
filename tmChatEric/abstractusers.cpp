#include "abstractusers.h"

AbstractUsers::AbstractUsers()
{
}

bool AbstractUsers::contains(quint32 uid)
{
    return users.contains(uid);
}

AbstractUser* AbstractUsers::user(quint32 uid)
{
    return users[uid];
}

QList<AbstractUser *> AbstractUsers::allUsers()
{
    return users.values();
}

quint32 AbstractUsers::length()
{
    return users.count();
}
