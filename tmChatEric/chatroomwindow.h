#ifndef CHATROOMWINDOW_H
#define CHATROOMWINDOW_H

#include <QWidget>
#include "types.h"

namespace Ui {
    class ChatRoomWindow;
}

class ChatRoomWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChatRoomWindow(QWidget *parent = 0);
    ~ChatRoomWindow();
    void setUserList(QList<UserInfo>);
    void addLine(QString text);
    void setTitle(QString name);
    void activate();
    void joinDenied(int reason, QString additional);

public slots:
        void returnPressed();
        void removeStatusbar();

signals:
    void textEntered(QString);
    void windowClosed();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::ChatRoomWindow *ui;
};

#endif // CHATROOMWINDOW_H
