#ifndef ABSTRACTCHATROOM_H
#define ABSTRACTCHATROOM_H

#include "abstractusers.h"

class AbstractChatRoom
{
public:
    AbstractChatRoom(quint32 id, QString name);
    quint32 id();
    QString name();

protected:
    quint32 _id;
    QString _name;

};

#endif // ABSTRACTCHATROOM_H
