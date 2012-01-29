#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "createchatroomdialog.h"

MainWindow::MainWindow(Client *client, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->chatRoomTable,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(cellSelected(int,int)));
    connect(ui->refreshButton, SIGNAL(clicked()), client, SLOT(sendBroadCast()));
    connect(ui->commandLine, SIGNAL(returnPressed()), SLOT(commandLineSlot()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setChatRoomInfo(QList<ChatRoomInfo*> chatRoomsInfo)
{
    ui->chatRoomTable->model()->removeRows(0,ui->chatRoomTable->rowCount());
    foreach(ChatRoomInfo * chatRoomInfo, chatRoomsInfo)
    {
        int rowCount = ui->chatRoomTable->rowCount();
        chatRoomIds.insert(rowCount, chatRoomInfo->id);
        ui->chatRoomTable->insertRow(rowCount);
        ui->chatRoomTable->setItem(rowCount, 0, new QTableWidgetItem(chatRoomInfo->address.toString()));
        ui->chatRoomTable->setItem(rowCount, 1, new QTableWidgetItem(QString::number(chatRoomInfo->id)));
        ui->chatRoomTable->setItem(rowCount, 2, new QTableWidgetItem(QString::number(chatRoomInfo->numberOfUsers)));
        ui->chatRoomTable->setItem(rowCount, 3, new QTableWidgetItem(chatRoomInfo->name));
    }
}

void MainWindow::cellSelected(int row, int col)
{
    (void)col;
    emit chatRoomSelected(row);
}

void MainWindow::on_actionCreate_ChatRoom_triggered()
{
    CreateChatRoomDialog * dialog = new CreateChatRoomDialog();
    connect(dialog,SIGNAL(createChatRoom(QString)),SIGNAL(createChatRoom(QString)));
    dialog->exec();
}

void MainWindow::commandLineSlot()
{
    QString command = ui->commandLine->text();
    ui->commandLine->clear();
    if (command.startsWith("add ip "))
    {
        QHostAddress add(right(command, "add ip "));
        emit addIp(add);
        ui->statusBar->showMessage("IP Address " + add.toString() + " added", 5000);
    }
    else if (command.startsWith("set null "))
    {
        QString next = right(command, "set null ");
        bool client = true;
        bool server = true;
        if (next.startsWith("client "))
        {
            next = right(next, "client ");
            server = false;
        }
        else if (next.startsWith("server "))
        {
            next = right(next, "server ");
            client = false;
        }

        if (next != "0" && next != "1") {
            ui->statusBar->showMessage("Unknown command: " + command, 5000);
            return;
        }

        bool enable = (next == "1");

        if (client)
            emit enableClientKeepalive(enable);
        if (server)
            emit enableServerKeepalive(enable);

        QString message = "Keepalives ";
        message += enable ? "enabled " : "disabled ";
        message += "for ";
        if (client && server)
            message += "Client and Server";
        else
            message += client ? "Client" : "Server";

        ui->statusBar->showMessage(message, 5000);
    }
    else
        ui->statusBar->showMessage("Unknown command: " + command, 5000);
}

void MainWindow::clearChatRoomInfo()
{
    ui->chatRoomTable->model()->removeRows(0,ui->chatRoomTable->rowCount());
}

QString MainWindow::right(QString &input, const char cutoffLeft[])
{
    return input.right(input.length() - QString(cutoffLeft).length());
}

void MainWindow::closeEvent(QCloseEvent *)
{
    exit(0);
}
