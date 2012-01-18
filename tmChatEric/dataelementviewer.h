#ifndef DATAUNITVIEWER_H
#define DATAUNITVIEWER_H

#include <QMainWindow>
#include "dataelement.h"
#include <QHostAddress>

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

private:
    static DataElementViewer * instance;
    explicit DataElementViewer(QWidget *parent = 0);
    ~DataElementViewer();


    Ui::DataUnitViewer *ui;
    struct Message {
        Message (ClientServer clientServer, Direction direction, Protocol protocol, const QHostAddress & address, DataElement * data) :
            _rawData(data->rawData()),
            _serverClient(clientServer == Client ? "Client" : "Server"),
            _direction(direction == In ? "In" : "Out"),
            _protocol(protocol == Tcp ? "Tcp" : (protocol == UdpUnicast ? "UdpUnicast" : "Broadcast")),
            _channel(QString::number(data->chatRoomIdentifier())),
            _type(QString::number(data->type())),
            _subType(QString::number(data->subType())),
            _message(data->message()),
            _validMessage("?"),
            _address(address.toString()),

            clientServer(clientServer),
            direction(direction),
            protocol(protocol),
            channel(data->chatRoomIdentifier()),
            type(data->type()),
            subType(data->subType()),
            address(address)
        {

        }

        QString _rawData;
        QString _serverClient;
        QString _direction;
        QString _protocol;
        QString _channel;
        QString _type;
        QString _subType;
        QString _validType;
        QString _message;
        QString _validMessage;
        QString _address;

        ClientServer clientServer;
        Direction direction;
        Protocol protocol;
        int channel;
        int type;
        int subType;
        QHostAddress address;
    };

    QList<Message> messages;

    void append(const Message & m);

private slots:
    void update();
};

#endif // DATAUNITVIEWER_H
