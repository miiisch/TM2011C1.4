#ifndef CHATSOCKET_H
#define CHATSOCKET_H

#include <QObject>
#include <QHostAddress>
#include "dataelement.h"

class QTcpSocket;
class QUdpSocket;

class ChatSocket : public QObject
{
    Q_OBJECT
public:
    explicit ChatSocket(QTcpSocket * socket, quint32 userId, QObject *parent = 0);
    ChatSocket(QUdpSocket * socket, QObject *parent = 0);
    bool send(DataElement data);
    QHostAddress ip();
    quint32 userId();
    quint32 setUserId(quint32 userId);
    int timeOutCounter();
    void incrementTimeOutCounter();
    void resetTimeOutCounter();

signals:
    void newTcpData(DataElement data, quint32 userId);
    void newUdpData(DataElement data, QHostAddress * senderAddress);

public slots:
    void readUdpData();
    void readTcpData();

private:
    bool checkMagicNumber(QByteArray magicNumber);
    quint32 readLength(QByteArray length);
    QTcpSocket * tcpSocket;
    QUdpSocket * udpSocket;
    enum State{
        Idle,
        WaitingForLength,
        WaitingForContent,
        CorruptMagicNumber
    };
    enum Type{
        TCP,
        UDP
    };
    Type type;
    quint32 contentLength;
    State currentState;
    quint32 _userId;
    bool everyThingRead;
    int _timeOutCounter;
};

#endif // CHATSOCKET_H
