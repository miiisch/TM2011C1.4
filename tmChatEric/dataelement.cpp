#include "dataelement.h"
#include <QDataStream>
#include <QIODevice>
#include <QDebug>
#include <QBuffer>

DataElement::DataElement(quint32 chatRoomIdentifier, quint32 type, quint32 subType, quint32 sender, quint32 receiver) :
    _contentLength(0), _chatRoomIdentifier(chatRoomIdentifier), _type(type), _subType(subType), _sender(sender), _receiver(receiver), _message(QByteArray())

{
    QDataStream ds(&_data, QIODevice::WriteOnly);
    ds << (quint8)'T' << (quint8)'M' << (quint8)'2' << (quint8)'0' << (quint8)'1' << (quint8)'1' << (quint8)'C' << (quint8)'1';
    ds << _contentLength;
    ds << _chatRoomIdentifier;
    ds << _type;
    ds << _subType;
    ds << _sender;
    ds << _receiver;
    messageDataStream = new QDataStream(&_message,QIODevice::ReadWrite);
}

DataElement::DataElement(QByteArray ba)
{
    QByteArray prefix;
    QDataStream dsPreFix(&prefix, QIODevice::WriteOnly);
    dsPreFix << (quint8)'T' << (quint8)'M' << (quint8)'2' << (quint8)'0' << (quint8)'1' << (quint8)'1' << (quint8)'C' << (quint8)'1';
    dsPreFix << _contentLength;

    _data.append(ba);
    QDataStream ds(&_data, QIODevice::ReadOnly);
    ds >> _chatRoomIdentifier;
    ds >> _type;
    ds >> _subType;
    ds >> _sender;
    ds >> _receiver;
    quint64 pos = ds.device()->pos();
    _message = _data.right(_data.length() - pos);
    messageDataStream = new QDataStream(&_message,QIODevice::ReadWrite);
    _data.prepend(prefix);
}

quint32 DataElement::type() const
{
    return _type;
}

void DataElement::setType(quint32 type)
{
    _type = type;
    QDataStream ds(&_data,QIODevice::WriteOnly);
    ds.device()->seek(16);
    ds << _type;
}

quint32 DataElement::subType() const
{
    return _subType;
}

void DataElement::setSubType(quint32 subType)
{
    _subType = subType;
    QDataStream ds(&_data,QIODevice::WriteOnly);
    ds.device()->seek(20);
    ds << _subType;
}

quint32 DataElement::chatRoomIdentifier() const
{
    return _chatRoomIdentifier;
}

quint32 DataElement::sender() const
{
    return _sender;
}

quint32 DataElement::receiver() const
{
    return _receiver;
}


QByteArray DataElement::message() const
{
    return _message;
}

QByteArray DataElement::data()
{
    QDataStream ds(&_data,QIODevice::WriteOnly);
    ds.device()->seek(8);
    _contentLength = _message.length();
    ds << _contentLength;
    return _data;
}

DataElement::operator QString() const
{
    return QString("ChatroomId: %1, Type: %2, SubType: %3, S: %4, R: %5").arg(_chatRoomIdentifier).arg(_type).arg(_subType).arg(_sender).arg(_receiver);
}

QString DataElement::readString()
{
    quint32 length;
    *messageDataStream >> length;

    char c[length+1];
    messageDataStream->readRawData(c,length);
    c[length] = 0;
    QString str;
    str = QString::fromUtf8(c);
    return str;
}

quint32 DataElement::readInt32()
{
    quint32 value;
    *messageDataStream >> value;
    return value;
}

quint16 DataElement::readInt16()
{
    quint16 value;
    *messageDataStream >> value;
    return value;
}

quint8 DataElement::readInt8()
{
    quint8 value;
    *messageDataStream >> value;
    return value;
}

void DataElement::writeString(QString string)
{
    QByteArray ba;
    QDataStream ds(&ba, QIODevice::WriteOnly);
    QByteArray stringUtf8 = string.toUtf8();
    quint32 length = (quint32)stringUtf8.length();
    ds << length;
    ba.append(stringUtf8);
    _data.append(ba);
    _message.append(ba);
}

void DataElement::writeInt32(quint32 value)
{
    QByteArray ba;
    QDataStream ds(&ba, QIODevice::WriteOnly);
    ds << value;
    _data.append(ba);
    _message.append(ba);
}

void DataElement::writeInt16(quint16 value)
{
    QByteArray ba;
    QDataStream ds(&ba, QIODevice::WriteOnly);
    ds << value;
    _data.append(ba);
    _message.append(ba);
}

void DataElement::writeInt8(quint8 value)
{
    QByteArray ba;
    QDataStream ds(&ba, QIODevice::WriteOnly);
    ds << value;
    _data.append(ba);
    _message.append(ba);
}

void DataElement::append(QByteArray ba)
{
    _data.append(ba);
    _message.append(ba);
}

void DataElement::updateLength()
{
    quint32 length = _data.length();
    QDataStream ds(&_data, QIODevice::WriteOnly);
    ds.device()->seek(8);
    ds << length;
}
