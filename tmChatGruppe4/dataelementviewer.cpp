#include "dataelementviewer.h"
#include "ui_dataelementviewer.h"
#include <QTableWidget>
#include <QDebug>
#include <QTime>
#include <QHeaderView>

#define DATAELEMENT_HEADER "|----Magic Number-----| |-Length--| |-Channel-| |--Type---| |-Subtype-| |-Sender--| |Reciever-|"

DataElementViewer* DataElementViewer::instance = 0;

DataElementViewer::DataElementViewer(QWidget *parent) :
    QMainWindow(parent),
    log("log"),
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
    connect(ui->outputTable, SIGNAL(currentCellChanged(int,int,int,int)), SLOT(showDetailedInformation(int)));

    QHeaderView * v = ui->outputTable->horizontalHeader();
    int i = 0;
    v->resizeSection(i++, 75);
    v->resizeSection(i++, 130);
    v->resizeSection(i++, 60);
    v->resizeSection(i++, 40);
    v->resizeSection(i++, 100);
    v->resizeSection(i++, 40);
    v->resizeSection(i++, 100);
    v->resizeSection(i++, 100);
    v->resizeSection(i++, 50);
    v->resizeSection(i++, 50);
    v->resizeSection(i++, 40);
    v->resizeSection(i++, 40);

    log.open(QIODevice::WriteOnly | QIODevice::Truncate);
    ds = new QTextStream(&log);
    log.flush();
}

DataElementViewer::~DataElementViewer()
{
    delete ui;
}

DataElementViewer * DataElementViewer::getInstance()
{
    if (instance == 0)
    {
        qDebug("create DEV");
        instance = new DataElementViewer;
    }
    return instance;
}

void DataElementViewer::addMessage(ClientServer clientServer, Direction direction, Protocol protocol, const QHostAddress & address, DataElement * data)
{
    QTime time = QTime::currentTime();
    Message m(clientServer, direction, protocol, address, data, time);
    messages += m;
    append(m);
    *ds << "[" << m._time << "] Channel " << m._channel << " (" << m._type << " | " << m._subType << ") " << m._sender << " -> " << m._receiver << endl;
    *ds << DATAELEMENT_HEADER << endl;
    *ds << m._rawDataHex << endl;
    *ds << m._rawDataChar << endl;
    *ds << m._message << endl << endl;
    log.flush();
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

        t->setItem(row, col++, new QTableWidgetItem(m._time));
        t->setItem(row, col++, new QTableWidgetItem(m._address));
        t->setItem(row, col++, new QTableWidgetItem(m._serverClient));
        t->setItem(row, col++, new QTableWidgetItem(m._direction));
        t->setItem(row, col++, new QTableWidgetItem(m._protocol));
        t->setItem(row, col++, new QTableWidgetItem(m._channel));
        t->setItem(row, col++, new QTableWidgetItem(m._type));
        t->setItem(row, col++, new QTableWidgetItem(m._subType));
        t->setItem(row, col++, new QTableWidgetItem(m._sender));
        t->setItem(row, col++, new QTableWidgetItem(m._receiver));
        t->setItem(row, col++, new QTableWidgetItem(m._validType));
        t->setItem(row, col++, new QTableWidgetItem(m._validMessage));
        t->setItem(row, col++, new QTableWidgetItem(m._message));
    }
}

void DataElementViewer::showDetailedInformation(int index)
{
    if (index < 0 || index >= messages.length())
    {
        ui->rawDataDescription->clear();
        ui->rawDataHex->clear();
        ui->rawDataChar->clear();
        ui->formattedContent->clear();
        return;
    }
    Message m = messages[index];
    ui->rawDataDescription->setText(DATAELEMENT_HEADER);
    ui->rawDataHex->setText(m._rawDataHex);
    ui->rawDataChar->setText(m._rawDataChar);
    ui->formattedContent->setText(m._message);
}

