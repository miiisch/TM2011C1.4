#ifndef STDINREADER_H
#define STDINREADER_H

#include <QObject>


class StdinReaderHelper;


class StdinReader : public QObject
{
    Q_OBJECT
public:
    StdinReader(QObject *parent);
    ~StdinReader();
    StdinReaderHelper *reader;
    QString prompt;
public slots:
    void setPrompt(QString);
signals:
    void startReader(); // you do not need to call this.
    void lineAvailable(QString);
private slots:
    void printPrompt();
};

class StdinReaderHelper : public QObject
{
    Q_OBJECT
public:
    StdinReaderHelper() : QObject(0) {}
signals:
    void lineAvailable(QString);
public slots:
    void startReading();
private:
    QByteArray buf;
};


#endif // STDINREADER_H
