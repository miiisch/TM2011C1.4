#include "createchatroomdialog.h"
#include "ui_createchatroomdialog.h"

CreateChatRoomDialog::CreateChatRoomDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateChatRoomDialog)
{
    ui->setupUi(this);
}

CreateChatRoomDialog::~CreateChatRoomDialog()
{
    delete ui;
}

void CreateChatRoomDialog::on_buttonBox_accepted()
{
    emit createChatRoom(ui->lineEdit->text());
}
