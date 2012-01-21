#include "chatroomwindow.h"
#include "ui_chatroomwindow.h"
#include <QTimer>

ChatRoomWindow::ChatRoomWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatRoomWindow)
{
    ui->setupUi(this);
    ui->textInput->setEnabled(false);
    ui->chatRoomStatus->setStyleSheet("background-color: yellow;");
    ui->chatRoomStatus->setText("Waiting for server");
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
        ui->userList->insertItem(row, new QListWidgetItem(QString::number(info.id) + " " + info.name));
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
    setWindowTitle(name);
}

void ChatRoomWindow::activate()
{
    ui->chatRoomStatus->setText("Connection established");
    ui->chatRoomStatus->setStyleSheet("background-color: #7fff00;");
    ui->textInput->setEnabled(true);
    QTimer::singleShot(2000, this, SLOT(removeStatusbar()));
}

void ChatRoomWindow::removeStatusbar()
{
    ui->chatRoomStatus->setVisible(false);
}

void ChatRoomWindow::joinDenied(int reason, QString additional)
{
    QString mainReason;
    switch(reason)
    {
    case 2:
        mainReason = "No specific Reason";
        break;

    case 3:
        mainReason = "Username taken";
        break;

    case 4:
        mainReason = "Chatroom full";
        break;

    case 5:
        mainReason = "Not in whitelist";
        break;

    default:
        mainReason = "ERROR";
        break;
    }

    mainReason += additional.isEmpty() ? "" : " (" + additional + ")";
    ui->chatRoomStatus->setText("Join Denied: " + mainReason);
    ui->chatRoomStatus->setStyleSheet("background-color: red;");
}

void ChatRoomWindow::serverQuit()
{
    ui->chatRoomStatus->setVisible(true);
    ui->chatRoomStatus->setText("Server connection closed");
    ui->chatRoomStatus->setStyleSheet("background-color: red;");
    ui->textInput->setEnabled(false);
}
