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
    explicit ChatSocket(QTcpSocket * socket, quint32 userId, bool gui, QObject *parent);
    explicit ChatSocket(QUdpSocket * socket, bool gui, QObject *parent);

    bool send(DataElement data, bool server);
    QHostAddress ip();
    quint32 userId();
    void setUserId(quint32 userId);
    int timeOutCounter();
    void incrementTimeOutCounter();
    void resetTimeOutCounter();
    bool handShakeDone();
    void setHandShakeDone();
    QHostAddress localIp();

signals:
    void newTcpData(DataElement data, quint32 userId, QHostAddress);
    void newUdpData(DataElement data, QHostAddress * senderAddress, quint16 port, QUdpSocket* udpSocket);

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
    bool _handshakeDone;

    bool _gui;
};

#endif // CHATSOCKET_H
