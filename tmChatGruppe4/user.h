#ifndef USER_H
#define USER_H

#include <QTypeInfo>

class ChatSocket;

class User
{
public:
    User(quint32 uid, ChatSocket * socket);
    User();
    ~User();
    ChatSocket* socket();
    quint32 uid();
    QList<QString> modules;


private:
    quint32 _uid;
    ChatSocket * _socket;

};

#endif // USER_H
