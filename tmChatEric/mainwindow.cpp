#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "createchatroomdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->chatRoomTable,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(cellSelected(int,int)));
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
        ui->chatRoomTable->setItem(rowCount, 0, new QTableWidgetItem(QString::number(chatRoomInfo->id), Qt::NoItemFlags));
        ui->chatRoomTable->setItem(rowCount, 1, new QTableWidgetItem(chatRoomInfo->name,Qt::NoItemFlags));
        ui->chatRoomTable->setItem(rowCount, 2, new QTableWidgetItem(QString::number(chatRoomInfo->numberOfUsers), Qt::NoItemFlags));
    }
}

void MainWindow::cellSelected(int row, int col)
{
    (void)col;
    emit chatRoomSelected(chatRoomIds[row]);
}

void MainWindow::on_actionCreate_ChatRoom_triggered()
{
    CreateChatRoomDialog * dialog = new CreateChatRoomDialog();
    connect(dialog,SIGNAL(createChatRoom(QString)),SIGNAL(createChatRoom(QString)));
    dialog->exec();
}
