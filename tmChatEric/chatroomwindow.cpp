#include "chatroomwindow.h"
#include "ui_chatroomwindow.h"

ChatRoomWindow::ChatRoomWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatRoomWindow)
{
    ui->setupUi(this);
    connect(ui->textInput,SIGNAL(returnPressed()),SLOT(returnPressed()));
}

ChatRoomWindow::~ChatRoomWindow()
{
    delete ui;
}

void ChatRoomWindow::setUserList(QList<UserInfo> userInfo)
{
    ui->userList->clear();
    foreach(UserInfo info, userInfo)
    {
        int row = ui->userList->count();
        ui->userList->insertItem(row, new QListWidgetItem(QString::number(info.id) + info.name));
    }
}

void ChatRoomWindow::returnPressed()
{
    emit textEntered(ui->textInput->text());
    ui->textInput->clear();
}

void ChatRoomWindow::addLine(QString text)
{
    ui->textBrowser->append(text);
}

void ChatRoomWindow::closeEvent(QCloseEvent * e)
{
    emit windowClosed();
    QWidget::closeEvent(e);
}

void ChatRoomWindow::setTitle(QString name)
{
    ui->chatRoomName->setText(name);
}
