#include "commandline.h"
#include <QKeyEvent>
#include <QDebug>


CommandLine::CommandLine(QWidget *parent) :
    QLineEdit(parent)
{
    historyPointer = 0;
    inputHistory.append("");
    connect(this,SIGNAL(returnPressed()),SLOT(saveEnteredText()));
}

void CommandLine::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Down)
    {
        if(historyPointer < (inputHistory.count()-1) && historyPointer != -1)
        {
            historyPointer++;
            this->setText(inputHistory[historyPointer]);
        }
    } else if(e->key() == Qt::Key_Up)
    {
        if(historyPointer == (inputHistory.count()-1))
            inputHistory[historyPointer] = this->text();
        if(historyPointer > 0) {
            historyPointer--;
            this->setText(inputHistory[historyPointer]);
        }
    }

    QLineEdit::keyPressEvent(e);
}


void CommandLine::saveEnteredText()
{
    inputHistory[inputHistory.count()-1] = this->text();
    inputHistory.append("");
    historyPointer = inputHistory.count()-1;
}
