#ifndef TYPES_H
#define TYPES_H

#include <QHostAddress>

struct ChatRoomInfo{
    quint16 tcpPort;
    quint32 id;
    QString name;
    quint32 numberOfUsers;
    QHostAddress address;
    bool isLocalChatRoom;
    ChatRoomInfo(quint16 tcpPort, quint32 id, QString name, quint32 numberOfUsers, QHostAddress address, bool isLocalChatRoom)
        : tcpPort(tcpPort), id(id), name(name), numberOfUsers(numberOfUsers), address(address), isLocalChatRoom(isLocalChatRoom){}
};

enum Status{
    Online,
    Away,
    Busy
};

struct UserInfo{
    quint32 id;
    QString name;
    Status status;
    UserInfo() :id(-1), name("UNINITIALIZED"), status(Online) {}
    UserInfo(quint32 id, QString name, Status status) : id(id), name(name), status(status) {}
    bool operator==(UserInfo info) { return info.id == id && info.name == name; }
    operator QString() const {return QString("%1 %2").arg(QString::number(id), name);}
};

#endif // TYPES_H
