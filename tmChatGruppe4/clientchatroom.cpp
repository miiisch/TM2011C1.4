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
    if (!userInfo.contains(data.sender()))
    {
        qDebug() << "received message from unknown sender";
    }
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
    case 9:
        readActionAceptedMessage(data);
        break;
    case 10:
        readActionDeniedMessage(data);
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
            quint32 subType;
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
                window->setStatusMessage(x);
                return;
            }
            DataElement data(_id, 5, subType, _userId, 0);
            data.writeString(message);
            socket()->send(data, false);
            return;
        }
        else if (text.startsWith("/kick ")) {
            QString next = text.right(text.length() - QString("/kick ").length());
            quint32 uid;
            bool ok = splitInt(next, uid);
            if (!ok)
            {
                QString x = QString("invalid arg: %1").arg(next);
                window->setStatusMessage(x);
                return;
            }

            DataElement data(_id, 8, 2, _userId, uid);
            data.writeString(next);
            socket()->send(data, false);
            return;
        }
        else if (text.startsWith("/set kick ") || text.startsWith("/set mod "))
        {

            QString next;
            quint32 subType;
            if (text.startsWith("/set kick "))
            {
                next = text.right(text.length() - QString("/set kick ").length());
                subType = 0;
            }
            else
            {
                next = text.right(text.length() - QString("/set mod ").length());
                subType = 3;
            }


            quint32 giveRights;
            bool ok = splitInt(next, giveRights);
            if (!ok)
            {
                QString x = QString("invalid arg: %1").arg(next);
                window->setStatusMessage(x);
                return;
            }

            quint32 uid;
            ok = splitInt(next, uid);
            if (!ok)
            {
                QString x = QString("invalid arg: %1").arg(next);
                window->setStatusMessage(x);
                return;
            }

            if (giveRights == 0)
                subType++;
            else if (giveRights != 1)
            {
                QString x = QString("invalid arg: %1").arg(next);
                window->setStatusMessage(x);
                return;
            }

            DataElement data(_id, 8, subType, _userId, uid);
            data.writeString(next);
            socket()->send(data, false);
            return;
        }
        else
        {
            //try private message
            QString next = text.right(text.size() - 1);
            quint32 uid;
            if (!splitInt(next, uid))
            {
                QString x = QString("unkown command: %1").arg(text);
                window->setStatusMessage(x);
                return;
            }

            DataElement data(_id, 4, 0, _userId, uid);
            data.writeString(next);
            socket()->send(data, false);
            return;
        }
    }

    DataElement data(_id, 4, 0, _userId, 0);
    data.writeString(text);
    socket()->send(data, false);
}

void ClientChatRoom::showChatMessage(DataElement data, quint32)
{
    //qDebug() << senderId;
    QString sender = userInfo[data.sender()].name;
    if (data.receiver() == 0)
        window->addPublicChatMessage(sender, data.readString());
    else
    {
        QString receiver = userInfo[data.receiver()].name;
        window->addPrivateChatMessage(sender, receiver, data.readString());
    }

    //show message using libnotify, but only when sender wasn't the user itself
//    if(data.sender() != user && !window->hasFocus())
//    {
//<<<<<<< HEAD
//        NotifyNotification *notification = notify_notification_new(name.toUtf8().constData(), textMessage.toUtf8().constData(), 0, 0);
//=======
//        NotifyNotification *notification = notify_notification_new(name.toUtf8().constData(), textMessage.toUtf8().constData(), 0);
//>>>>>>> master
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
    quint32 reason = data.subType();
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

void ClientChatRoom::readActionAceptedMessage(DataElement data)
{
    QString sender = userInfo[data.sender()].name;
    QString receiver = userInfo[data.receiver()].name;
    switch(data.subType())
    {
    case 0:
        window->addActionMessage("%1 granted %2 kick rights", sender, receiver, data.readString());
        break;
    case 1:
        window->addActionMessage("%1 revoked kick rights from %2", sender, receiver, data.readString());
        break;
    case 2:
        window->addActionMessage("%2 was kicked by %1", sender, receiver, data.readString());
        if (userId() == data.receiver())
            disableChatroom("You were kicked");
        userInfo.remove(data.receiver());
        window->setUserList(userInfo);
        break;
    case 3:
        window->addActionMessage("%1 granted %2 mod rights", sender, receiver, data.readString());
        break;
    case 4:
        window->addActionMessage("%1 revoked mod rights from %2", sender, receiver, data.readString());
        break;
    }
}

void ClientChatRoom::readActionDeniedMessage(DataElement data)
{
    QString receiver = userInfo[data.receiver()].name;
    switch(data.subType())
    {
    case 0:
        window->addActionDeniedMessage("Server did not allow granting %1 kick rights", receiver, data.readString());
        break;
    case 1:
        window->addActionDeniedMessage("Server did not allow revoking kick rights from %1", receiver, data.readString());
        break;
    case 2:
        window->addActionDeniedMessage("Server did not allow kicking %1", receiver, data.readString());
        break;
    case 3:
        window->addActionDeniedMessage("Server did not allow granting %1 kick rights", receiver, data.readString());
        break;
    case 4:
        window->addActionDeniedMessage("Server did not allow revoking %1 mod rights", receiver, data.readString());
        break;
    }
}

void ClientChatRoom::disableChatroom(QString reason)
{
    emit closed(socket()->ip(), userId());
    window->setStatusMessage(reason, "red", 0);
    window->disableInput();
}

bool ClientChatRoom::splitInt(QString &s, quint32 &i)
{
    QList<QString> split = s.split(" ");
    if (split.isEmpty())
        return false;

    bool ok;
    i = split[0].toInt(&ok);
    if (!ok) {
        return false;
    }

    if (split.size() > 0)
        s = s.right(s.length() - split[0].size() - (split.size() > 1 ? 1 : 0));

    return true;
}
