#ifndef CHATROOMUSER_H
#define CHATROOMUSER_H

#include <QString>
#include "chatsocket.h"

class ChatRoomUser
{
public:
    enum Status{
        Online,
        Away,
        Busy
    };
    ChatRoomUser(ChatSocket* socket, quint32 uid, QString name, Status status);
    void setName(QString name);
    void setStatus(ChatRoomUser::Status status);
    QString name();
    Status status();
    quint32 uid();
    ChatSocket * socket();

private:
    quint32 _uid;
    ChatSocket * _socket;
    QString _name;
    Status _status;

};

#endif // CHATROOMUSER_H
