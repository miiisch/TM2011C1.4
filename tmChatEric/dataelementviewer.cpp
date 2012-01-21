#include "dataelementviewer.h"
#include "ui_dataelementviewer.h"
#include <QTableWidget>
#include <QDebug>

DataElementViewer* DataElementViewer::instance = 0;

DataElementViewer::DataElementViewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DataUnitViewer),
    channelMatcher(".*"),
    typeMatcher(".*"),
    subTypeMatcher(".*"),
    senderMatcher(".*"),
    receiverMatcher(".*"),
    addressMatcher(".*")
{
    ui->setupUi(this);

    connect(ui->applyButton, SIGNAL(clicked()), SLOT(update()));
}

DataElementViewer::~DataElementViewer()
{
    delete ui;
}

DataElementViewer * DataElementViewer::getInstance()
{
    if (instance == 0)
    {
        instance = new DataElementViewer;
        instance->show();
    }
    return instance;
}

void DataElementViewer::addMessage(ClientServer clientServer, Direction direction, Protocol protocol, const QHostAddress & address, DataElement * data)
{
    Message m(clientServer, direction, protocol, address, data);
    messages += m;
    append(m);
}

void DataElementViewer::update()
{
    channelMatcher = QRegExp(ui->filterChannel->text() == "" ? ".*" : ui->filterChannel->text());
    typeMatcher = QRegExp(ui->filterType->text() == "" ? ".*" : ui->filterType->text());
    subTypeMatcher = QRegExp(ui->filterSubType->text() == "" ? ".*" : ui->filterSubType->text());
    senderMatcher = QRegExp(ui->filterSender->text() == "" ? ".*" : ui->filterSender->text());
    receiverMatcher = QRegExp(ui->filterReceiver->text() == "" ? ".*" : ui->filterReceiver->text());
    addressMatcher = QRegExp(ui->filterHost->text() == "" ? ".*" : ui->filterHost->text());


   while(ui->outputTable->rowCount() != 0)
        ui->outputTable->removeRow(0);
    foreach (Message m, messages)
        append(m);
}

void DataElementViewer::append(const Message & m)
{
    bool filter = true;
    filter &= ui->filterIn->isChecked()             || m.direction != In;
    filter &= ui->filterOut->isChecked()            || m.direction != Out;

    filter &= ui->filterTcp->isChecked()            || m.protocol != Tcp;
    filter &= ui->filterUdpUnicast->isChecked()     || m.protocol != UdpUnicast;
    filter &= ui->filterUdpBroadcast->isChecked()   || m.protocol != UdpBroadcast;

    filter &= ui->filterClient->isChecked()         || m.clientServer != Client;
    filter &= ui->filterServer->isChecked()         || m.clientServer != Server;

    filter &= channelMatcher.exactMatch(m.channel);
    filter &= typeMatcher.exactMatch(m.type);
    filter &= subTypeMatcher.exactMatch(m.subType);
    filter &= senderMatcher.exactMatch(m.sender);
    filter &= receiverMatcher.exactMatch(m.receiver);
    filter &= addressMatcher.exactMatch(m.address);

    filter &= !ui->filterNullMessage->isChecked()   || !m.null;
    filter |= ui->filterInvalidType->isChecked()    && !m.validType;
    filter |= ui->filterInvalidMessage->isChecked() && !m.validMessage;

    if (filter)
    {
        QTableWidget * t = ui->outputTable;
        int row = t->rowCount();
        int col = 0;
        t->insertRow(row);

        t->setItem(row, col++, new QTableWidgetItem(m._rawData));
        t->setItem(row, col++, new QTableWidgetItem(m._serverClient));
        t->setItem(row, col++, new QTableWidgetItem(m._direction));
        t->setItem(row, col++, new QTableWidgetItem(m._protocol));
        t->setItem(row, col++, new QTableWidgetItem(m._channel));
        t->setItem(row, col++, new QTableWidgetItem(m._type));
        t->setItem(row, col++, new QTableWidgetItem(m._subType));
        t->setItem(row, col++, new QTableWidgetItem(m._sender));
        t->setItem(row, col++, new QTableWidgetItem(m._receiver));
        t->setItem(row, col++, new QTableWidgetItem(m._validType));
        t->setItem(row, col++, new QTableWidgetItem(m._message));
        t->setItem(row, col++, new QTableWidgetItem(m._validMessage));
        t->setItem(row, col++, new QTableWidgetItem(m._address));
    }
}

