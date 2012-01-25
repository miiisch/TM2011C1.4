#include "clientchatroom.h"
#include "chatroomwindow.h"

//#undef signals
//#include <libnotify/notification.h>

ClientChatRoom::ClientChatRoom(ChatSocket* socket, quint32 id, QString name, quint32 userId) :
    _id(id), _name(name), _socket(socket), _userId(userId)
{
    window = new ChatRoomWindow();
    connect(window,SIGNAL(textEntered(QString)),this,SLOT(sendMessage(QString)));
    connect(window,SIGNAL(windowClosed()),this,SLOT(sendUserQuit()));
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
    if (text == "")
            return;
    if (text.startsWith("//"))
        text = text.right(text.length() - 1);
    else if (text.startsWith("/"))
    {
        if (text.startsWith("/set status "))
        {
            QString arg = text.right(text.length() - QString("/set status ").length());
            int subType;
            QString message;
            if (arg.startsWith("online"))
            {
                subType = 0;
                if (arg.length() > QString("online").length())
                    message = arg.right(arg.length() - QString("online ").length());
            }
            else if (arg.startsWith("away"))
            {
                subType = 1;
                if (arg.length() > QString("away").length())
                    message = arg.right(arg.length() - QString("away ").length());
            }
            else if (arg.startsWith("busy"))
            {
                subType = 2;
                if (arg.length() > QString("busy").length())
                    message = arg.right(arg.length() - QString("busy ").length());
            }
            else
            {
                QString x = QString("unkown command: %1").arg(text);
                window->setErrorMessage(x);
                return;
            }
            DataElement data(_id, 5, subType, _userId, 0);
            data.writeString(message);
            socket()->send(data, false);
            return;
        }
        else
        {
            QString x = QString("unkown command: %1").arg(text);
            window->setErrorMessage(x);
            return;
        }
    }

    DataElement data(_id, 4, 0, _userId, 0);
    data.writeString(text);
    socket()->send(data, false);
}

void ClientChatRoom::showChatMessage(DataElement data, quint32 user)
{
    //qDebug() << senderId;
    QString name = "";
    if (data.receiver() != 0)
    {
        name += "[";
    }
    foreach(UserInfo info, userInfo)
    {
        if(info.id == data.sender())
        {
            name += info.name;
            break;
        }
    }
    if (data.receiver() != 0)
    {
        name += " -> ";
        foreach(UserInfo info, userInfo)
        {
            if(info.id == data.sender())
            {
                name += info.name;
                break;
            }
        }
        name = "]";
    }
    QString textMessage = data.readString();

    window->addPublicChatMessage(name, textMessage);

    //show message using libnotify, but only when sender wasn't the user itself
//    if(data.sender() != user && !window->hasFocus())
//    {
//        NotifyNotification *notification = notify_notification_new(name.toUtf8().constData(), textMessage.toUtf8().constData(), 0);
//        notify_notification_show(notification, 0);
//    }
}

void ClientChatRoom::showDenyMessage(DataElement data, quint32)
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

    window->addErrorMessage("Message sending denied", textMessage, denyMessage);
}

quint32 ClientChatRoom::id()
{
    return _id;
}

void ClientChatRoom::readStatusMessage(DataElement data)
{
    quint32 id = data.sender();
    QString string = data.readString();
    QString name = userInfo[id].name;
    if (data.subType() != 5 && !userInfo.contains(id)) // when not join message
        qDebug() << "received message from " << id << " which is not in channel " << _id;
    switch(data.subType())
    {
    case 5:
        //qDebug() << "User joined id: " << id << " name: " << string;
        userInfo[id] = UserInfo(id, string, Online);
        window->setUserList(userInfo);
        window->addStatusMessage("%1 joined", string);
        break;
    case 3:
    case 4:
    {
        //qDebug() << "User left id: " << id << " name: " << string;
        userInfo.remove(id);
        window->setUserList(userInfo);
        QString message = data.subType() == 3 ? "%1 quit" : "Connection with %1 lost";
        window->addStatusMessage(message, name, string);
        break;
    }
    case 0:
    {
        window->addStatusMessage("%1 is available", name, string);
        userInfo[id].status = Online;
        window->setUserList(userInfo);
        break;
    }
    case 1:
    {
        window->addStatusMessage("%1 is away", name, string);
        userInfo[id].status = Away;
        window->setUserList(userInfo);
        break;
    }
    case 2:
    {
        window->addStatusMessage("%1 is busy", name, string);
        userInfo[id].status = Busy;
        window->setUserList(userInfo);
        break;
    }
    }
}

void ClientChatRoom::sendUserQuit()
{
    emit closed(socket()->ip(), id());
    DataElement data(_id,5,3,_userId,0);
    data.writeString("Bye");
    _socket->send(data, false);
}

void ClientChatRoom::activate(QMap<quint32, UserInfo> userInfo)
{
    this->userInfo = userInfo;
    window->setUserList(userInfo);
    window->activate();
}

void ClientChatRoom::denyJoin(DataElement &data)
{
    int reason = data.subType();
    QString text = data.readString();
    window->joinDenied(reason, text);
}

void ClientChatRoom::serverQuit()
{
    window->serverQuit();
}

QString ClientChatRoom::name()
{
    return _name;
}
