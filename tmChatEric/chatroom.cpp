#include "chatroom.h"
#include "dataelement.h"
#include <QDebug>

ChatRoom::ChatRoom(quint32 id, QString name) :
    AbstractChatRoom(id, name)
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
        if(data.subType() <= 1)
            readChatMessage(data, userId);
        else
            qDebug() << "Unknown SubType";
        break;
    case 5:
        if(data.subType() > 0 && data.subType() <= 5)
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
    QString joinMessage = data.readString();
    ///qDebug() << "User(" << userName <<  ") joined channel " << _id << " with Message: " << joinMessage;
    users.addUser(socket, uid, userName, ChatRoomUser::Online);
    QList<ChatRoomUser*> allUsers = users.allUsers();
    ChatRoomUser* newUser = (ChatRoomUser*)users.user(uid);

    {

        //Join-Request answer
        DataElement newDataElement(_id,3,0);
        newDataElement.writeInt32(allUsers.length());
        foreach(ChatRoomUser* user, allUsers)
        {
            newDataElement.writeInt32(user->uid());
            newDataElement.writeInt32(user->status());
            newDataElement.writeString(user->name());
            //empty modules list
            newDataElement.writeInt32(0);
        }
        newUser->socket()->send(newDataElement);
    }

    {
        //User joined message to others
        foreach (ChatRoomUser* user, allUsers) {
            if(user != newUser) {
                DataElement newDataElement(_id,5,0);
                newDataElement.writeInt32(newUser->uid());
                newDataElement.writeString(newUser->name());
                user->socket()->send(newDataElement);
            }
        }
    }

}

void ChatRoom::readChatMessage(DataElement data, quint32 uid)
{
    //read userid
    quint32 userId = data.readInt32();
    if(userId != uid) {
        qDebug() << "corrupt chatmessage, user has send userId: " << userId << " but has id: " << uid;
        return;
    }

    switch(data.subType())
    {
    case 0:
        sendChatMessage(data);
        break;
    case 1:
        sendPrivateMessage(data);
        break;
    default:
        qDebug() << "Unknown Subtype";
    }

}

void ChatRoom::sendChatMessage(DataElement data)
{
    QList<ChatRoomUser*> allUsers = users.allUsers();
    foreach(ChatRoomUser* user, allUsers)
    {
        user->socket()->send(data);
    }
}

void ChatRoom::sendPrivateMessage(DataElement data)
{
    quint32 receiver = data.readInt32();
    users.user(receiver)->socket()->send(data);
}

void ChatRoom::readStatusMessage(DataElement data, quint32 uid)
{
    ChatRoomUser* currentUser = (ChatRoomUser*)users.user(uid);
    //Update serverside
    switch(data.subType())
    {
    case 1:
        currentUser->setName(data.readString());
        break;
    case 2:
        currentUser->setStatus(ChatRoomUser::Online);
        break;
    case 3:
        currentUser->setStatus(ChatRoomUser::Away);
        break;
    case 4:
        currentUser->setStatus(ChatRoomUser::Busy);
        break;
    case 5:
        users.remove(uid);
        break;
    }

    //send statusMessage to all
    QList<ChatRoomUser*> allUsers = users.allUsers();
    foreach(ChatRoomUser* user, allUsers)
    {
        user->socket()->send(data);
    }
}
