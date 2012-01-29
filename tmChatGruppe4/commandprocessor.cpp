#include "commandprocessor.h"
#include "server.h"
#include "client.h"
#include <iostream>
using namespace std;
#include "stdinreader.h"


CommandProcessor::CommandProcessor() :
    QObject(0),
    client(0),
    server(0),
    stdinReader(new StdinReader(this))
{
    connect(stdinReader, SIGNAL(lineAvailable(QString)), SLOT(processCommand(QString)));
    stdinReader->setPrompt("tmChat> ");
}

void CommandProcessor::setClient(Client *c)
{
    //qDebug("client");
    client = c;
}

void CommandProcessor::setServer(Server *s)
{
    //qDebug("server");
    server = s;
}

QString CommandProcessor::right(QString &input, const char cutoffLeft[])
{
    return input.right(input.length() - QString(cutoffLeft).length());
}

bool CommandProcessor::splitInt(QString &s, quint32 &i)
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

void CommandProcessor::processCommand(QString command)
{
    if (command.startsWith("add ip "))
    {
        QString address = right(command, "add ip ");
        QHostAddress add(address);
        client->addIp(add);
        writeStatus("IP Address " + add.toString() + " added");
    }
    else if (command.startsWith("set null "))
    {
        QString next = right(command, "set null ");
        bool typeClient = true;
        bool typeServer = true;
        if (next.startsWith("client "))
        {
            next = right(next, "client ");
            typeServer = false;
        }
        else if (next.startsWith("server "))
        {
            next = right(next, "server ");
            typeClient = false;
        }

        if (next != "0" && next != "1") {
            writeStatus("Unknown command: " + command);
            return;
        }

        bool enable = (next == "1");

        if(typeClient)
            client->enableKeepalivesClient(enable);
        if(typeServer)
            client->enableKeepalivesServer(enable);

        QString message = "Keepalives ";
        message += enable ? "enabled " : "disabled ";
        message += "for ";
        if (typeClient && typeServer)
            message += "Client and Server";
        else
            message += typeClient ? "Client" : "Server";

        writeStatus(message);
    }
    else if (command.startsWith("set deny "))
    {
        QString next = right(command, "set deny ");
        QString message = "Deny everything %1";
        if (next == "1")
        {
            client->denyAllServer(true);
            writeStatus(message.arg("enabled"));
        }
        else if (next == "0")
        {
            client->denyAllServer(false);
            writeStatus(message.arg("disabled"));
        }
        else
        {
            writeStatus("Unknown command: " + command);
            return;
        }
    }
    else if (command.startsWith("create "))
    {
        QString next = right(command, "create ");
        if(client)
            client->createChatRoom(next);
        else if(server)
            server->createChatRoom(next);
    }
    else if (command.startsWith("close "))
    {
        quint32 id;
        QString next = right(command, "close ");
        bool valid = splitInt(next, id);
        if (!valid)
        {
            writeStatus("Unknown command: " + command);
            return;
        }
        client->closeChatRoom(id, next);
    }
    else
        writeStatus("Unknown command: " + command);

}

void CommandProcessor::writeStatus(QString text)
{
    //if (_gui)
    //    emit writeToStatusbar(text, 5000);
//    else
    cout << qPrintable(text) << endl;
}
