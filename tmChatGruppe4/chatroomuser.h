#ifndef CHATROOMUSER_H
#define CHATROOMUSER_H

#include <QString>

class ChatSocket;

class ChatRoomUser
{
public:
    enum Status{
        Online,
        Away,
        Busy
    };
    ChatRoomUser(ChatSocket* socket, quint32 uid, QString name, Status status, bool moderatorPermission , bool kickPermission);
    void setName(QString name);
    void setStatus(ChatRoomUser::Status status);
    QString name();
    Status status();
    quint32 uid();
    ChatSocket * socket();
    bool moderatorPermission;
    bool kickPermission;


private:
    quint32 _uid;
    ChatSocket * _socket;
    QString _name;
    Status _status;

};

#endif // CHATROOMUSER_H
