#include <QtGui/QApplication>
#include "mainwindow.h"
#include "client.h"
#include "server.h"

#include <QTcpSocket>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    quint16 serverPort = 0;
    if (argc == 3)
    {
        bool ok;
        serverPort = QString(argv[2]).toInt(&ok);
        if (!ok)
            qFatal("Usage: %s username [server port]", argv[0]);
    }
    else
         qFatal("Usage: %s username [server port]", argv[0]);

    new Client(argv[1], serverPort);

    return a.exec();
}
