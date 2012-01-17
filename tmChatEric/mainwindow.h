#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "client.h"
#include <QHash>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setChatRoomInfo(QList<ChatRoomInfo*> chatRoomsInfo);

public slots:
    void cellSelected(int row, int col);

signals:
    void chatRoomSelected(quint32);
    void createChatRoom(QString);

private slots:
    void on_actionCreate_ChatRoom_triggered();

private:
    Ui::MainWindow *ui;
    QHash<int, quint32> chatRoomIds;
};

#endif // MAINWINDOW_H