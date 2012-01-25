#include "chatroom.h"
#include "dataelement.h"
#include <QDebug>

ChatRoom::ChatRoom(quint32 id, QString name) :
    _id(id), _name(name)
{
}


quint32 ChatRoom::chatRoomUsersCount()
{
    return users.length();
}

void ChatRoom::newData(ChatSocket* socket, DataElement data, quint32 userId)
{
    switch(data.type())
    {
    case 3:
        if(data.subType() == 0)
            readJoinRequest(socket, data, userId);
        else
            qDebug() << "Unknown SubType";
        break;
    case 4:
        if(data.subType() == 0)
            readChatMessage(data, userId);
        else
            qDebug() << "Unknown SubType";
        break;
    case 5:
        if(data.subType() <= 3)
            readStatusMessage(data, userId);
        else
            qDebug() << "Unknown SubType";
        break;

    default:
        qDebug() << "Unknown Type";
        break;
    }
}

void ChatRoom::readJoinRequest(ChatSocket* socket, DataElement data, quint32 uid)
{
    QString userName = data.readString();
    bool found = false;
    foreach (ChatRoomUser * u, users.allUsers())
    {
        if (u->name() == userName)
        {
            found = true;
            break;
        }
    }

    if (found) {
        DataElement newDataElement(_id,3,3,uid,0);
        socket->send(newDataElement, true);
        return;
    }

    QString joinMessage = data.readString();
    ///qDebug() << "User(" << userName <<  ") joined channel " << _id << " with Message: " << joinMessage;
    users.addUser(socket, uid, userName, ChatRoomUser::Online);
    QList<ChatRoomUser*> allUsers = users.allUsers();
    ChatRoomUser* newUser = (ChatRoomUser*)users.user(uid);

    {

        //Join-Request answer
        DataElement newDataElement(_id,3,1,uid,0);
        newDataElement.writeInt32(allUsers.length());
        foreach(ChatRoomUser* user, allUsers)
        {
            newDataElement.writeInt32(user->uid());
            newDataElement.writeInt32(user->status());
            newDataElement.writeString(user->name());
            //empty modules list
            newDataElement.writeInt32(0);
        }
        newUser->socket()->send(newDataElement, true);
    }

    {
        //User joined message to others
        foreach (ChatRoomUser* user, allUsers) {
            if(user != newUser) {
                DataElement newDataElement(_id,6,5,uid,0);
                newDataElement.writeString(newUser->name());
                //empty modules list
                newDataElement.writeInt32(0);
                user->socket()->send(newDataElement, true);
            }
        }
    }

}

void ChatRoom::readChatMessage(DataElement data, quint32)
{
    switch(data.receiver())
    {
    case 0:
        sendChatMessage(data);
        break;
    default:
        sendPrivateMessage(data);
        break;
    }

}

void ChatRoom::sendChatMessage(DataElement data)
{
    data.setSubType(1);
    QList<ChatRoomUser*> allUsers = users.allUsers();
    foreach(ChatRoomUser* user, allUsers)
    {
        user->socket()->send(data, true);
    }
}

void ChatRoom::sendPrivateMessage(DataElement data)
{
    data.setSubType(2);
    users.user(data.receiver())->socket()->send(data, true);
    users.user(data.sender())->socket()->send(data, true); // send message to sender (ack)
}

void ChatRoom::readStatusMessage(DataElement data, quint32 uid)
{
    ChatRoomUser* currentUser = (ChatRoomUser*)users.user(uid);
    //Update serverside
    switch(data.subType())
    {
//    case X:
//        currentUser->setName(data.readString());
//        break;
    case 0:
        currentUser->setStatus(ChatRoomUser::Online);
        break;
    case 1:
        currentUser->setStatus(ChatRoomUser::Away);
        break;
    case 2:
        currentUser->setStatus(ChatRoomUser::Busy);
        break;
    case 3:
        users.remove(uid);
        break;
    }

    //send statusMessage to all
    data.setType(6);
    QList<ChatRoomUser*> allUsers = users.allUsers();
    foreach(ChatRoomUser* user, allUsers)
    {
        user->socket()->send(data, true);
    }
}

void ChatRoom::userConnectionLost(quint32 uid)
{
    if (users.contains(uid))
    {
        users.remove(uid);
        DataElement data(id(), 6, 4, uid, 0);
        QList<ChatRoomUser*> allUsers = users.allUsers();
        foreach(ChatRoomUser* user, allUsers)
        {
            user->socket()->send(data, true);
        }
    }
}

quint32 ChatRoom::id()
{
    return _id;
}

QString ChatRoom::name()
{
    return _name;
}