#include "stdinreader.h"
#include <iostream>
#include <QThread>
#include <cstdio>

StdinReader::StdinReader(QObject *parent)
{
    reader = new StdinReaderHelper();

    QThread *readerThread = new QThread(this);
    reader->moveToThread(readerThread);
    readerThread->start();

    connect(this, SIGNAL(startReader()), reader, SLOT(startReading()));
    emit startReader();
    connect(reader, SIGNAL(lineAvailable(QString)), SIGNAL(lineAvailable(QString)));
    connect(reader, SIGNAL(lineAvailable(QString)), SLOT(printPrompt())); //executed AFTER lineAvailable, so the prompt gets printed after processing the input line ;)
}

StdinReader::~StdinReader()
{
    delete reader;
}

void StdinReader::setPrompt(QString p)
{
    prompt = p;
    printPrompt();
}

void StdinReader::printPrompt()
{
    std::cout << "\x1b[1m";
    std::cout << prompt.toStdString();
    std::cout << "\x1b[0m";
    fflush(stdout);
}




void StdinReaderHelper::startReading()
{
    buf.clear();
    forever
    {
        char c = getchar();

        if(c == '\n')
        {
            fflush(stdout);
            emit lineAvailable(QString::fromUtf8(buf));
            buf.clear();
        }
        else
            buf.append(c);
    }
}
