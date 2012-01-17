#include "abstractchatroom.h"

AbstractChatRoom::AbstractChatRoom(quint32 id, QString name) :
    _id(id), _name(name)
{
}

quint32 AbstractChatRoom::id()
{
    return _id;
}

QString AbstractChatRoom::name()
{
    return _name;
}



