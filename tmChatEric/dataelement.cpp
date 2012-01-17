#include "dataelement.h"
#include <QDataStream>
#include <QIODevice>
#include <QDebug>
#include <QBuffer>

DataElement::DataElement(quint32 chatRoomIdentifier, quint32 type, quint32 subType) :
    _contentLength(0), _chatRoomIdentifier(chatRoomIdentifier), _type(type), _subType(subType), _message(QByteArray())

{
    QDataStream ds(&_data, QIODevice::WriteOnly);
    ds << (quint8)'T' << (quint8)'M' << (quint8)'2' << (quint8)'0' << (quint8)'1' << (quint8)'1' << (quint8)'C' << (quint8)'1';
    ds << _contentLength;
    ds << _chatRoomIdentifier;
    ds << _type;
    ds << _subType;
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
    quint64 pos = ds.device()->pos();
    _message = _data.right(_data.length() - pos);
    messageDataStream = new QDataStream(&_message,QIODevice::ReadWrite);
    _data.prepend(prefix);
}

quint32 DataElement::type()
{
    return _type;
}

quint32 DataElement::subType()
{
    return _subType;
}

quint32 DataElement::chatRoomIdentifier()
{
    return _chatRoomIdentifier;
}

QByteArray DataElement::message()
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
    return QString("ChatroomId: %1, Type: %2, SubType: %3").arg(_chatRoomIdentifier).arg(_type).arg(_subType);
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
