#ifndef CREATECHATROOMDIALOG_H
#define CREATECHATROOMDIALOG_H

#include <QDialog>

namespace Ui {
    class CreateChatRoomDialog;
}

class CreateChatRoomDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateChatRoomDialog(QWidget *parent = 0);
    ~CreateChatRoomDialog();

signals:
    void createChatRoom(QString);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::CreateChatRoomDialog *ui;
};

#endif // CREATECHATROOMDIALOG_H
