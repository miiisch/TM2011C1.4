#include "chatroomwindow.h"
#include "ui_chatroomwindow.h"
#include <QTimer>
#include <QTextDocument>

ChatRoomWindow::ChatRoomWindow(QWidget *parent) :
    QWidget(parent),
    statusbarTimerCounter(0),
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

void ChatRoomWindow::setUserList(QMap<quint32, UserInfo> userInfo)
{
    ui->userList->clear();
    foreach(UserInfo info, userInfo)
    {
        int row = ui->userList->count();
        QString status = info.status == Online ? "+" : (info.status == Away ? "-" : "!");
        ui->userList->insertItem(row, new QListWidgetItem(QString::number(info.id) + " " + status + " " + info.name));
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
    if (ui->textInput->isEnabled())
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
    setRemoveStatusbarTimer(5000);
}

void ChatRoomWindow::removeStatusbar()
{
    statusbarTimerCounter--;
    if (statusbarTimerCounter == 0)
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

void ChatRoomWindow::setStatusMessage(QString &message, QString bgColor, int timeOut)
{
    ui->chatRoomStatus->setVisible(true);
    ui->chatRoomStatus->setText(message);
    ui->chatRoomStatus->setStyleSheet(QString("background-color: %1;").arg(bgColor));
    setRemoveStatusbarTimer(timeOut);
}

void ChatRoomWindow::setRemoveStatusbarTimer(int timeOut)
{
    if (timeOut != 0)
    {
        statusbarTimerCounter++;
        QTimer::singleShot(timeOut, this, SLOT(removeStatusbar()));
    }
}

void ChatRoomWindow::addPublicChatMessage(QString sender, QString message)
{
    addLine(QString("<b>%1:</b> %2").arg(Qt::escape(sender), Qt::escape(message)));
}

void ChatRoomWindow::addPrivateChatMessage(QString sender, QString receiver, QString message)
{
    addLine(QString("<b>[%1 -&gt; %2]:</b> %3").arg(Qt::escape(sender), Qt::escape(receiver), Qt::escape(message)));
}

void ChatRoomWindow::addStatusMessage(QString text, QString sender)
{
    addStatusMessage(text, sender, "");
}

// text should contain %1 where sender is inserted
void ChatRoomWindow::addStatusMessage(QString text, QString sender, QString reason)
{
    addLine(QString("<i>%1%2</i>").arg(Qt::escape(text)).arg(Qt::escape(sender), reason.isEmpty() ? "" : Qt::escape(QString(" (%1)").arg(reason))));
}

// text should contain %1 and %2 for sender and receiver
void ChatRoomWindow::addActionMessage(QString text, QString sender, QString receiver, QString reason)
{
    addLine(QString("<i>%1%3</i>").arg(Qt::escape(text)).arg(Qt::escape(sender), Qt::escape(receiver), reason.isEmpty() ? "" : Qt::escape(QString(" (%1)").arg(reason))));
}

void ChatRoomWindow::addErrorMessage(QString what, QString message, QString reason)
{
    addLine(QString("<b>%1:</b> %2%3").arg(what, Qt::escape(message), reason.isEmpty() ? "" : Qt::escape(QString(" (%1)").arg(reason))));
}

void ChatRoomWindow::disableInput()
{
    ui->textInput->setEnabled(false);
}
