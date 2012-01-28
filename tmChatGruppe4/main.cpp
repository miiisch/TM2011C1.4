#include <QtGui/QApplication>
#include "mainwindow.h"
#include "client.h"
#include "server.h"

#include <QTcpSocket>
//#undef signals
//#include <libnotify/notify.h>
//#include <libnotify/notification.h>

int main(int argc, char *argv[])
{
    QCoreApplication * a;

//    if(notify_init("tmChatGruppe4"))
//        qDebug("libnotify initialized!");

    QString usage = QString("Usage: %s <username> [-p <port>] [--no-gui]").arg(argv[0]);

    quint16 port = 0;
    bool gui = true;

    if (argc < 2)
        qFatal("%s", qPrintable(usage));
    QString name = argv[1];
    for (int i = 0; i < argc; ++i)
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

    if (gui)
    {
        a = new QApplication(argc, argv);
        new Client(name, port);
    }
    else
    {
        a = new QCoreApplication(argc, argv);
        new Server(port, true, false, false);
    }

    return a->exec();
}
