#ifndef CHATROOMS_H
#define CHATROOMS_H

#include <QHash>
#include "dataelement.h"

class ChatRoom;
class ChatSocket;

class ChatRooms
{
public:
    ChatRooms();
    void newData(ChatSocket* socket, DataElement data, quint32 userId);
    QMap<quint32, QPair<QString, quint32> > chatRoomsInfo();
    void addChatRoom(QString name);
    void userConnectionLost(quint32 uid);

private:
    quint32 chatRoomIdCounter;
    QHash<quint32, ChatRoom*> chatRooms;
};

#endif // CHATROOMS_H
