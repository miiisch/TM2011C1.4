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
    QApplication a(argc, argv);

//    if(notify_init("tmChatGruppe4"))
//        qDebug("libnotify initialized!");

    quint16 serverPort = 0;
    if (argc == 3)
    {
        bool ok;
        serverPort = QString(argv[2]).toInt(&ok);
        if (!ok)
            qFatal("Usage: %s username [server port]", argv[0]);
    }
    else if (argc != 2)
         qFatal("Usage: %s username [server port]", argv[0]);

    new Client(argv[1], serverPort);

    return a.exec();
}
