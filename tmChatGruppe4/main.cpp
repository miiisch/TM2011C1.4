#include <QtGui/QApplication>
#include "mainwindow.h"
#include "client.h"
#include "server.h"
#include "commandprocessor.h"

#include <QTcpSocket>
//#undef signals
//#include <libnotify/notify.h>
//#include <libnotify/notification.h>


int main(int argc, char *argv[])
{
    QCoreApplication * a;

//    if(notify_init("tmChatGruppe4"))
//        qDebug("libnotify initialized!");

    QString usage = QString("Usage: %1 <username> [-p <port>] [--no-gui]").arg(argv[0]);

    quint16 port = 0;
    bool gui = true;

    if (argc < 2)
        qFatal("%s", qPrintable(usage));
    QString name = QString::fromUtf8(argv[1]);
    for (int i = 2; i < argc; ++i)
    {
        if (qstrcmp(argv[i], "-p") == 0)
        {
            i++;
            if (argc == i)
                qFatal("%s", qPrintable(usage));
            QString portString = argv[i];
            bool ok;
            port = portString.toInt(&ok);
            if (!ok)
                qFatal("%s", qPrintable(usage));
        }
        else if (qstrcmp(argv[i], "--no-gui") == 0)
        {
            gui = false;
        }
        else
            qFatal("%s", qPrintable(usage));
    }

    CommandProcessor * cmd = new CommandProcessor();

    if(gui)
    {
        a = new QApplication(argc, argv);
        Client * client = new Client(name, port);
        CommandProcessor * cmd = new CommandProcessor();
        cmd->setClient(client);
        QObject::connect(client, SIGNAL(processCommand(QString)), cmd, SLOT(processCommand(QString)));
        QObject::connect(client, SIGNAL(serverCreated(Server*)), cmd, SLOT(setServer(Server*)));
    }
    else
    {
        a = new QCoreApplication(argc, argv);
        Server * server = new Server(port, true, true, false, false);
        cmd->setServer(server);
    }

    return a->exec();
}
