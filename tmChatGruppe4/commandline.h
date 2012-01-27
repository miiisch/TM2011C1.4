#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <QLineEdit>

class CommandLine : public QLineEdit
{
    Q_OBJECT
public:
    explicit CommandLine(QWidget *parent = 0);

signals:


protected:
    void keyPressEvent(QKeyEvent *);

private slots:
    void saveEnteredText();

private:
    QStringList inputHistory;
    int historyPointer;
    bool isWriting;
};

#endif // COMMANDLINE_H
