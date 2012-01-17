#ifndef ABSTRACTUSER_H
#define ABSTRACTUSER_H

#include <QTypeInfo>
#include "chatsocket.h"

class AbstractUser
{
public:
    AbstractUser(ChatSocket* socket, quint32 uid);
    quint32 uid();
    ChatSocket* socket();

protected:
    quint32 _uid;
    ChatSocket* _socket;
};

#endif // ABSTRACTUSER_H
