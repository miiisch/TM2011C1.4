#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include <QObject>

class Client;
class Server;
class StdinReader;

class CommandProcessor : public QObject
{
    Q_OBJECT
public:
    explicit CommandProcessor();

signals:

public slots:
    void setClient(Client*);
    void setServer(Server*);
    void processCommand(QString command);
    void writeStatus(QString text);

private:
    Client *client;
    Server *server;
    StdinReader *stdinReader;

    QString right(QString &input, const char cutoffLeft[]);
    bool splitInt(QString &s, quint32 &i);
};

#endif // COMMANDPROCESSOR_H
