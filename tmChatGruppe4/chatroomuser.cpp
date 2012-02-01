#include "chatroomuser.h"
#include "chatsocket.h"

ChatRoomUser::ChatRoomUser(ChatSocket *socket, quint32 uid, QString name, Status status, bool moderatorPermission, bool kickPermission) :
    moderatorPermission(moderatorPermission), kickPermission(kickPermission), _uid(uid), _socket(socket), _name(name), _status(status)
{
}

void ChatRoomUser::setName(QString name)
{
    _name = name;
}

void ChatRoomUser::setStatus(ChatRoomUser::Status status)
{
    _status = status;
}

QString ChatRoomUser::name()
{
    return _name;
}

ChatRoomUser::Status ChatRoomUser::status()
{
    return _status;
}

quint32 ChatRoomUser::uid()
{
    return _uid;
}

ChatSocket * ChatRoomUser::socket()
{
    return _socket;
}
