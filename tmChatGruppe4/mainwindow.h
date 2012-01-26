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
    explicit MainWindow(Client * client, QWidget *parent = 0);
    ~MainWindow();
    void setChatRoomInfo(QList<ChatRoomInfo*> chatRoomsInfo);
    void clearChatRoomInfo();

public slots:
    void cellSelected(int row, int col);

signals:
    void chatRoomSelected(quint32);
    void createChatRoom(QString);
    void addIp(QHostAddress);
    void enableClientKeepalive(bool);
    void enableServerKeepalive(bool);
    void enableDenyAll(bool);

private slots:
    void on_actionCreate_ChatRoom_triggered();
    void commandLineSlot();
    void showViewer();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::MainWindow *ui;
    QHash<int, quint32> chatRoomIds;

    QString right(QString & input, const char cutoffLeft[]);
};

#endif // MAINWINDOW_H
