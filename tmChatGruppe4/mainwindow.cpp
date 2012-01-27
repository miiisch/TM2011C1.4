#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "createchatroomdialog.h"
#include "dataelementviewer.h"

MainWindow::MainWindow(Client *client, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->chatRoomTable,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(cellSelected(int,int)));
    connect(ui->refreshButton, SIGNAL(clicked()), client, SLOT(sendBroadCast()));
    connect(ui->commandLine, SIGNAL(returnPressed()), SLOT(commandLineSlot()));
    connect(ui->viewerButton, SIGNAL(clicked()), SLOT(showViewer()));
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
        ui->chatRoomTable->setItem(rowCount, 0, new QTableWidgetItem(chatRoomInfo->address.toString(), Qt::NoItemFlags));
        ui->chatRoomTable->setItem(rowCount, 1, new QTableWidgetItem(QString::number(chatRoomInfo->id), Qt::NoItemFlags));
        ui->chatRoomTable->setItem(rowCount, 2, new QTableWidgetItem(QString::number(chatRoomInfo->numberOfUsers), Qt::NoItemFlags));
        ui->chatRoomTable->setItem(rowCount, 3, new QTableWidgetItem(chatRoomInfo->name,Qt::NoItemFlags));
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

void MainWindow::showViewer()
{
    DataElementViewer::getInstance()->show();
}

bool MainWindow::splitInt(QString &s, quint32 &i)
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

void MainWindow::setCommandLineText(QString text, int timeout)
{
    ui->statusBar->showMessage(text, timeout);
}
