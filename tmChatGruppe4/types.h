#ifndef TYPES_H
#define TYPES_H

#include <QHostAddress>

struct ChatRoomInfo{
    quint16 tcpPort;
    quint32 id;
    QString name;
    quint32 numberOfUsers;
    QHostAddress address;
    ChatRoomInfo(quint16 tcpPort, quint32 id, QString name, quint32 numberOfUsers, QHostAddress address)
        : tcpPort(tcpPort), id(id), name(name), numberOfUsers(numberOfUsers), address(address){}
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
    bool moderatorPermission;
    bool kickPermission;
    UserInfo() :id(-1), name("UNINITIALIZED"), moderatorPermission(false), kickPermission(false), status(Online) {}
    UserInfo(quint32 id, QString name, Status status) : id(id), name(name), status(status), moderatorPermission(false), kickPermission(false) {}
    bool operator==(UserInfo info) { return info.id == id && info.name == name; }
};

#endif // TYPES_H
