#include <QDebug>
#include "user.h"
#include "chatsocket.h"

User::User(quint32 uid, ChatSocket *socket) :
    _uid(uid), _socket(socket)
{
}

User::User() :
        _uid(0), _socket(0)
{
}

User::~User()
{
    delete _socket;
    qDebug() << "Bye UserObject";
}

ChatSocket * User::socket()
{
    return _socket;
}


quint32 User::uid()
{
    return _uid;
}
