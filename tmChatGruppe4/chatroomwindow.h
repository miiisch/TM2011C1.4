#ifndef CHATROOMWINDOW_H
#define CHATROOMWINDOW_H

#include <QWidget>
#include "types.h"

class QTextDocument;

namespace Ui {
    class ChatRoomWindow;
}

class ChatRoomWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChatRoomWindow(QWidget *parent = 0);
    ~ChatRoomWindow();
    void setUserList(QMap<quint32, UserInfo>);
    void addPublicChatMessage(QString sender, QString message);
    void addPrivateChatMessage(QString sender, QString receiver, QString message);
    void addStatusMessage(QString text, QString sender);
    void addStatusMessage(QString text, QString sender, QString reason);
    void addActionMessage(QString text, QString sender, QString receiver, QString reason);
    void addActionDeniedMessage(QString text, QString receiver, QString reason);
    void addErrorMessage(QString what, QString message, QString reason);

    void setTitle(QString name);
    void activate();
    void joinDenied(int reason, QString additional);
    void serverQuit();
    void setStatusMessage(QString message, QString bgColor = "red", int timeOut = 5000);
    void disableInput();

public slots:
    void returnPressed();
    void removeStatusbar();

signals:
    void textEntered(QString);
    void windowClosed(bool, QString);

protected:
    void closeEvent(QCloseEvent *);

private:
    int statusbarTimerCounter;
    bool statusbarStayForever;
    Ui::ChatRoomWindow *ui;

    void addLine(QString);
};

#endif // CHATROOMWINDOW_H
