#ifndef DATAUNITVIEWER_H
#define DATAUNITVIEWER_H

#include <QMainWindow>
#include "dataelement.h"
#include <QHostAddress>
#include <QRegExp>
#include <QFile>
#include <QTextStream>
#include <QTime>

#define TYPE(X) if(data->type()==X)
#define SUBTYPE(X) if(data->subType()==X)
#define SUBTYPERANGE(X,Y) if(data->subType()>=X&&data->subType()<=Y)
#define INT INT32
#define INT32 message+=QString::number(data->readInt32(&validMessage))+" ";
#define INT16 message+=QString::number(data->readInt16(&validMessage))+" ";
#define INT8 message+=QString::number(data->readInt8(&validMessage))+" ";
#define STRING message+="\""+data->readString(&validMessage)+"\" ";
#define LIST(X) message += "[";\
    int length = data->readInt32(&validMessage);\
    for (int i = 0; i < length; ++i){\
        /*if (i != 0) message += ", ";*/\
        X}\
    message += "] ";
#define TUPLE(X) message += "(";\
    X\
    message += ") ";
#define MODULE TUPLE(INT32 STRING)
#define CHANNEL TUPLE(INT STRING INT)
#define USER TUPLE(INT INT STRING LIST(MODULE))

namespace Ui {
    class DataUnitViewer;
}

class DataElementViewer : public QMainWindow
{
    Q_OBJECT

public:
    enum ClientServer {
        Client,
        Server
    };

    enum Direction {
        In,
        Out
    };

    enum Protocol {
        Tcp,
        UdpUnicast,
        UdpBroadcast
    };

    static DataElementViewer * getInstance();
    void addMessage(ClientServer clientServer, Direction direction, Protocol protocol, const QHostAddress & address, DataElement * data);

private slots:
    void showDetailedInformation(int index);

private:
    static DataElementViewer * instance;
    explicit DataElementViewer(QWidget *parent = 0);
    ~DataElementViewer();
    QFile log;
    QTextStream * ds;


    Ui::DataUnitViewer *ui;
    struct Message {
        Message (ClientServer clientServer, Direction direction, Protocol protocol, const QHostAddress & address, DataElement * data, QTime time) :
            _time(time.toString()),
            _rawDataHex(data->rawDataHex()),
            _rawDataChar(data->rawDataChar()),
            _serverClient(clientServer == Client ? "Client" : "Server"),
            _direction(direction == In ? "In" : "Out"),
            _protocol(protocol == Tcp ? "Tcp" : (protocol == UdpUnicast ? "UdpUnicast" : "Broadcast")),
            _channel(QString::number(data->chatRoomIdentifier())),
            _sender(QString::number(data->sender())),
            _receiver(QString::number(data->receiver())),
            _address(address.toString()),

            clientServer(clientServer),
            direction(direction),
            protocol(protocol),
            channel(QString::number(data->chatRoomIdentifier())),
            type(QString::number(data->type())),
            subType(QString::number(data->subType())),
            sender(QString::number(data->sender())),
            receiver(QString::number(data->receiver())),
            address(address.toString()),
            null(isNullMessage(data))
        {
            getMessageInformation(data, _type, _subType, validType, _message, validMessage);
            _validType = validType ? "" : "!!";
            _validMessage = validMessage ? "" : "!!";
        }

        // Strings to put into table
        QString _time;
        QString _rawDataHex;
        QString _rawDataChar;
        QString _serverClient;
        QString _direction;
        QString _protocol;
        QString _channel;
        QString _type;
        QString _subType;
        QString _sender;
        QString _receiver;
        QString _validType;
        QString _message;
        QString _validMessage;
        QString _address;

        // to Match against filters
        ClientServer clientServer;
        Direction direction;
        Protocol protocol;
        QString channel;
        QString type;
        QString subType;
        QString sender;
        QString receiver;
        QString address;
        bool null;
        bool validType;
        bool validMessage;

        bool isNullMessage(DataElement * data)
        {
            return data->chatRoomIdentifier() == 0
                    && data->type() == 1
                    && data->subType() == 0
                    && data->sender() == 0
                    && data->receiver() == 0;
        }

        void getMessageInformation(DataElement * data, QString & type, QString & subType, bool & validType, QString & message, bool & validMessage)
        {
            validType = false;
            validMessage = true;
            message = "";

            TYPE(0)
            {
                type = "0 Hello";
                SUBTYPE(0)
                {
                    subType = "0 Request";
                    validType = data->sender() == 0 && data->receiver() == 0;
                }
                else SUBTYPE(1)
                {
                    subType = "1 Response";
                    validType = data->sender() == 0 && data->receiver() == 0;
                    INT16;
                    LIST(CHANNEL);
                }
            }
            else TYPE(1)
            {
                type = "1 Null";
                SUBTYPE(0)
                {
                    subType = "0 Null";
                    validType = data->sender() == 0 && data->receiver() == 0;
                }
            }
            else TYPE(2)
            {
                type = "2 Handshake";
                SUBTYPE(0)
                {
                    subType = "0 Request";
                    validType = data->sender() == 0 && data->receiver() == 0;
                    LIST(MODULE);
                }
                else SUBTYPE(1)
                {
                    subType = "1 Response";
                    validType = data->sender() == 0 && data->receiver() == 0;
                    INT;
                    LIST(MODULE);
                }
            }
            else TYPE(3)
            {
                type = "3 Join Channel";
                SUBTYPE(0)
                {
                    subType = "0 Request";
                    validType = data->sender() != 0 && data->receiver() == 0;
                    STRING;
                    STRING;
                }
                else SUBTYPE(1)
                {
                    subType = "1 Granted";
                    validType = data->sender() != 0 && data->receiver() == 0;
                    LIST(USER);
                }
                else SUBTYPERANGE(2,5)
                {
                    subType = QString::number(data->subType()) + " Denied";
                    validType = data->sender() != 0 && data->receiver() == 0;
                    STRING;
                }
            }
            else TYPE(4)
            {
                type = "4 Chat Message";
                SUBTYPE(0)
                {
                    subType = "0 Request";
                    validType = data->sender() != 0;
                    STRING;
                }
                else SUBTYPE(1)
                {
                    subType = "1 Distribution";
                    validType = data->sender() != 0;
                    STRING;
                }
                else SUBTYPE(2)
                {
                    subType = "2 Rejected";
                    validType = data->sender() != 0;
                    STRING;
                    STRING;
                }
            }
            else TYPE(5)
            {
                type = "5 Status Change Request";
                SUBTYPE(0)
                {
                    subType = "0 Available";
                    validType = data->sender() != 0 && data->receiver() == 0;
                    STRING;
                }
                else SUBTYPE(1)
                {
                    subType = "1 Away";
                    validType = data->sender() != 0 && data->receiver() == 0;
                    STRING;
                }
                else SUBTYPE(2)
                {
                    subType = "2 Busy";
                    validType = data->sender() != 0 && data->receiver() == 0;
                    STRING;
                }
                else SUBTYPE(3)
                {
                    subType = "3 Quit";
                    validType = data->sender() != 0 && data->receiver() == 0;
                    STRING;
                }
            }
            else TYPE(6)
            {
                type = "6 Status Change Notification";
                SUBTYPE(0)
                {
                    subType = "0 Available";
                    validType = data->sender() != 0 && data->receiver() == 0;
                    STRING;
                }
                else SUBTYPE(1)
                {
                    subType = "1 Away";
                    validType = data->sender() != 0 && data->receiver() == 0;
                    STRING;
                }
                else SUBTYPE(2)
                {
                    subType = "2 Busy";
                    validType = data->sender() != 0 && data->receiver() == 0;
                    STRING;
                }
                else SUBTYPE(3)
                {
                    subType = "3 Quit";
                    validType = data->sender() != 0 && data->receiver() == 0;
                    STRING;
                }
                else SUBTYPE(4)
                {
                    subType = "4 Connection Lost";
                    validType = data->sender() != 0 && data->receiver() == 0;
                }
                else SUBTYPE(5)
                {
                    subType = "5 Joined";
                    validType = data->sender() != 0 && data->receiver() == 0;
                    STRING;
                    LIST(MODULE);
                }
            }
            else TYPE(7)
            {
                type = "7 Channel closing";
                SUBTYPE(0)
                {
                    subType = "0";
                    validType = data->sender() == 0 && data->receiver() == 0;
                }
            }
            else TYPE(8)
            {
                type = "8 Perform Action";
                SUBTYPE(0)
                {
                    subType = "0 Grant Kick Right";
                    validType = data->sender() != 0 && data->receiver() != 0;
                    STRING;
                }
                else SUBTYPE(1)
                {
                    subType = "1 Revoke Kick Right";
                    validType = data->sender() != 0 && data->receiver() != 0;
                    STRING;
                }
                else SUBTYPE(2)
                {
                    subType = "2 Kick";
                    validType = data->sender() != 0 && data->receiver() != 0;
                    STRING;
                }
                else SUBTYPE(3)
                {
                    subType = "3 Grant Mod Right";
                    validType = data->sender() != 0 && data->receiver() != 0;
                    STRING;
                }
                else SUBTYPE(4)
                {
                    subType = "0 Revoke Mod Right";
                    validType = data->sender() != 0 && data->receiver() != 0;
                    STRING;
                }
            }
            else TYPE(9)
            {
                type = "9 Action Notification";
                SUBTYPE(0)
                {
                    subType = "0 Grant Kick Right";
                    validType = data->sender() != 0 && data->receiver() != 0;
                    STRING;
                }
                else SUBTYPE(1)
                {
                    subType = "1 Revoke Kick Right";
                    validType = data->sender() != 0 && data->receiver() != 0;
                    STRING;
                }
                else SUBTYPE(2)
                {
                    subType = "2 Kick";
                    validType = data->sender() != 0 && data->receiver() != 0;
                    STRING;
                }
                else SUBTYPE(3)
                {
                    subType = "3 Grant Mod Right";
                    validType = data->sender() != 0 && data->receiver() != 0;
                    STRING;
                }
                else SUBTYPE(4)
                {
                    subType = "0 Revoke Mod Right";
                    validType = data->sender() != 0 && data->receiver() != 0;
                    STRING;
                }
            }
            else TYPE(10)
            {
                type = "10 Action Rejected";
                SUBTYPE(0)
                {
                    subType = "0 Grant Kick Right";
                    validType = data->sender() != 0 && data->receiver() != 0;
                    STRING;
                }
                else SUBTYPE(1)
                {
                    subType = "1 Revoke Kick Right";
                    validType = data->sender() != 0 && data->receiver() != 0;
                    STRING;
                }
                else SUBTYPE(2)
                {
                    subType = "2 Kick";
                    validType = data->sender() != 0 && data->receiver() != 0;
                    STRING;
                }
                else SUBTYPE(3)
                {
                    subType = "3 Grant Mod Right";
                    validType = data->sender() != 0 && data->receiver() != 0;
                    STRING;
                }
                else SUBTYPE(4)
                {
                    subType = "0 Revoke Mod Right";
                    validType = data->sender() != 0 && data->receiver() != 0;
                    STRING;
                }
            }
            else TYPE(1023)
            {
                type = "0xFF Debug";
                subType = QString::number(data->subType());
                validType = data->sender() == 0 && data->receiver() == 0;
                STRING;
            }

            else
            {
                type = QString::number(data->type());
                subType = QString::number(data->subType());
            }

            // message only valid if at end
            validMessage &= data->rewind();
        }
    };

    QList<Message> messages;

    QRegExp channelMatcher;
    QRegExp typeMatcher;
    QRegExp subTypeMatcher;
    QRegExp senderMatcher;
    QRegExp receiverMatcher;
    QRegExp addressMatcher;

    void append(const Message & m, int index);

private slots:
    void update();
};

#endif // DATAUNITVIEWER_H
