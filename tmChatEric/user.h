#ifndef USER_H
#define USER_H

#include <QTypeInfo>
#include "abstractuser.h"

class ChatSocket;

class User
{
public:
    User(quint32 uid, ChatSocket * socket);
    User();
    ~User();
    ChatSocket* socket();
    quint32 uid();


private:
    quint32 _uid;
    ChatSocket * _socket;

};

#endif // USER_H
