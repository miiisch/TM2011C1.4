#include "abstractuser.h"

AbstractUser::AbstractUser(ChatSocket* socket, quint32 uid) :
    _socket(socket), _uid(uid)
{

}

quint32 AbstractUser::uid()
{
    return _uid;
}

ChatSocket * AbstractUser::socket()
{
    return _socket;
}
