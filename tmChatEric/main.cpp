#include <QtGui/QApplication>
#include "mainwindow.h"
#include "client.h"
#include "server.h"

#include <QTcpSocket>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(argc != 3)
        qFatal("please sumit 2 arguments");

    Client * client = new Client(argv[1]);

    return a.exec();
}
