#include <QtGui/QApplication>
#include "mainwindow.h"
#include "client.h"
#include "server.h"

#include <QTcpSocket>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    quint16 serverPort;
    if (argc == 2)
        serverPort = 0;
    else if (argc == 3)
    {
        bool ok;
        serverPort = QString(argv[2]).toInt(&ok);
        if (!ok)
            qFatal("Arguments: username [server port]");
    }
    else
        qFatal("Arguments: username [server port]");


    Client * client = new Client(argv[1], serverPort);

    return a.exec();
}
