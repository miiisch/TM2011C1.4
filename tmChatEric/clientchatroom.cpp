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
        if(data.subType() == 1)
        {
            qDebug() << "recieved ack";
            showChatMessage(data, userId);
        } else if (data.subType() == 2)
        {
            showDenyMessage(data, userId);
        }
        break;
    case 6:
        readStatusMessage(data);
        break;
    default:
        qDebug() << "ClientChatRoom::newData unknown type: " << data.type();
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
    DataElement data(_id, 4, 0, _userId, 0);
    data.writeString(text);
    socket()->send(data);
}

void ClientChatRoom::showChatMessage(DataElement data, quint32 userId)
{
    //qDebug() << senderId;
    QString name = "";
    foreach(UserInfo info, userInfo)
    {
        if(info.id == data.sender())
        {
            name = info.name;
            break;
        }
    }
    QString textMessage = data.readString();

    window->addLine("<b>" + name + ":</b> " + textMessage);
}

void ClientChatRoom::showDenyMessage(DataElement data, quint32 userId)
{
    //qDebug() << senderId;
    QString name = "";
    foreach(UserInfo info, userInfo)
    {
        if(info.id == data.sender())
        {
            name = info.name;
            break;
        }
    }
    QString textMessage = data.readString();
    QString denyMessage = data.readString();

    window->addLine("<b>Message sending denied:</b> " + textMessage + "<b>(" + denyMessage + ")</b>");
}

quint32 ClientChatRoom::id()
{
    return _id;
}

void ClientChatRoom::readStatusMessage(DataElement data)
{
    quint32 id = data.sender();
    QString string = data.readString();
    switch(data.subType())
    {
    case 5:
        //qDebug() << "User joined id: " << id << " name: " << string;
        userInfo.append(UserInfo(id, string, Online));
        window->setUserList(userInfo);
        break;
    case 3:
    case 4:
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
    DataElement data(_id,5,3,_userId,0);
    data.writeString("Bye");
    _socket->send(data);
}
