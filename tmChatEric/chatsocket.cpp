#include "chatsocket.h"
#include <QAbstractSocket>
#include <QHostAddress>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTimer>

ChatSocket::ChatSocket(QTcpSocket *socket, quint32 userId, QObject *parent) :
    QObject(parent), tcpSocket(socket), type(TCP), currentState(Idle), _userId(userId)
{
    connect(this->tcpSocket,SIGNAL(readyRead()),SLOT(readTcpData()));
}

ChatSocket::ChatSocket(QUdpSocket *socket, QObject *parent) :
    QObject(parent), udpSocket(socket), type(UDP), currentState(Idle)
{

    connect(this->udpSocket,SIGNAL(readyRead()),SLOT(readUdpData()));
}

void ChatSocket::readUdpData()
{
    qDebug() << "received UDP data";
    QHostAddress * senderAddress = new QHostAddress;
    quint16 * senderPort = new quint16;
    if(udpSocket->hasPendingDatagrams())
    {
        everyThingRead = false;
        qint64 size = udpSocket->pendingDatagramSize();
        char data[size];
        udpSocket->readDatagram(data,size,senderAddress,senderPort);
        //qDebug() << "read from port: " << *senderPort << " and address " << *senderAddress;
        QByteArray ba(data, size);
        if(!checkMagicNumber(ba.left(8)))
        {
            return;
        }

        //remove magicnumber and length
        ba.remove(0,12);
        DataElement dataElement(ba);

        //new Data signal
        emit newUdpData(dataElement, senderAddress, *senderPort);

        //check for next datagram
        QTimer::singleShot(0,this,SLOT(readUdpData()));
    } else if(!everyThingRead)
    {
        //check for next datagram
        QTimer::singleShot(0,this,SLOT(readUdpData()));
        everyThingRead = true;
    }
    return;
}

void ChatSocket::readTcpData()
{
    if(tcpSocket->bytesAvailable())
    {
        everyThingRead = false;
        switch(currentState)
        {
        case Idle:
            if(tcpSocket->bytesAvailable() >= 8)
            {
                if(checkMagicNumber(tcpSocket->read(8)))
                    currentState = WaitingForLength;
                else
                    currentState = CorruptMagicNumber;
            }
            break;
        case WaitingForLength:
            if(tcpSocket->bytesAvailable() >= 4)
            {
                contentLength = readLength(tcpSocket->read(4));
                currentState = WaitingForContent;
            }
            break;
        case WaitingForContent:
            if(tcpSocket->bytesAvailable() >= (contentLength + 20))
            {
                emit newTcpData(DataElement(tcpSocket->read(contentLength+20)), _userId);
                currentState = Idle;
            }
            break;
        case CorruptMagicNumber:
            tcpSocket->close();
            deleteLater();
            break;
        default:
            qFatal("Socket State unknown");
        }
        QTimer::singleShot(0,this,SLOT(readTcpData()));
    } else if(!everyThingRead)
    {
        QTimer::singleShot(0,this,SLOT(readTcpData()));
        everyThingRead = true;
    }
}

bool ChatSocket::checkMagicNumber(QByteArray magicNumber)
{
    if(magicNumber != QString("TM2011C1"))
    {
        qDebug() << "No valid dataElement (Magic number incorrect)";
        return false;
    }
    return true;
}

quint32 ChatSocket::readLength(QByteArray length)
{
    quint32 l;
    QDataStream ds(&length, QIODevice::ReadOnly);
    ds >> l;
    return l;

}

bool ChatSocket::send(DataElement data)
{
    if(type == TCP)
    {
        if(tcpSocket->isOpen()) {
            tcpSocket->write(data.data());
            return true;
        } else {
            return false;
        }
    } else {
        qFatal("only use chatsocket::send with tcp");
        return false;
    }
}

QHostAddress ChatSocket::ip()
{
    if(type == TCP)
    {
        return tcpSocket->peerAddress();
    }
}

quint32 ChatSocket::userId()
{
    return _userId;
}

quint32 ChatSocket::setUserId(quint32 userId)
{
    _userId = userId;
}

int ChatSocket::timeOutCounter()
{
    return _timeOutCounter;
}

void ChatSocket::incrementTimeOutCounter()
{
    _timeOutCounter++;
}

void ChatSocket::resetTimeOutCounter()
{
    _timeOutCounter = 0;
}
