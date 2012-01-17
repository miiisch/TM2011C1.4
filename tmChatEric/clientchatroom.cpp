#include "clientchatroom.h"
#include "chatroomwindow.h"

ClientChatRoom::ClientChatRoom(ChatSocket* socket, quint32 id, QString name, quint32 userId, QList<UserInfo> userInfo) :
    AbstractChatRoom(id, name), _socket(socket), _userId(userId), userInfo(userInfo)
{
    window = new ChatRoomWindow();
    connect(window,SIGNAL(textEntered(QString)),this,SLOT(sendMessage(QString)));
    connect(window,SIGNAL(windowClosed()),this,SLOT(sendUserQuit()));
    window->setUserList(userInfo);
    window->setTitle(_name);
    window->show();
}

void ClientChatRoom::newData(DataElement data, quint32 userId)
{
    switch(data.type())
    {
    case 4:
        if(data.subType() == 0)
        {
            showChatMessage(data, userId);
        }
        break;
    case 5:
        readStatusMessage(data);
        break;
    }
}

ChatSocket* ClientChatRoom::socket()
{
    return _socket;
}

quint32 ClientChatRoom::userId()
{
    return _userId;
}

void ClientChatRoom::sendMessage(QString text)
{
    DataElement data(_id, 4, 0);
    data.writeInt32(_userId);
    data.writeString(text);
    socket()->send(data);
}

void ClientChatRoom::showChatMessage(DataElement data, quint32 userId)
{
    quint32 senderId = data.readInt32();
    //qDebug() << senderId;
    QString name = "";
    foreach(UserInfo info, userInfo)
    {
        if(info.id == senderId)
        {
            name = info.name;
            break;
        }
    }
    QString textMessage = data.readString();

    window->addLine("<b>" + name + ":</b> " + textMessage);
}

quint32 ClientChatRoom::id()
{
    return _id;
}

void ClientChatRoom::readStatusMessage(DataElement data)
{
    quint32 id = data.readInt32();
    QString string = data.readString();
    switch(data.subType())
    {
    case 0:
        //qDebug() << "User joined id: " << id << " name: " << string;
        userInfo.append(UserInfo(id, string, Online));
        window->setUserList(userInfo);
        break;
    case 5:
        //qDebug() << "User left id: " << id << " name: " << string;
        for(int i=0;i<userInfo.length(); ++i)
        {
            if(userInfo[i].id == id)
            {
                userInfo.removeAt(i);
                break;
            }
        }
        window->setUserList(userInfo);
        break;
    }
}

void ClientChatRoom::sendUserQuit()
{
    DataElement data(_id,5,5);
    data.writeInt32(_userId);
    data.writeString("Bye");
    _socket->send(data);
}
