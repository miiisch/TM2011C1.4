#ifndef DATAELEMENT_H
#define DATAELEMENT_H

#include <QString>

typedef quint32 userId;

class DataElement
{
public:
    DataElement(quint32 chatRoomIdentifier, quint32 type, quint32 subType);
    DataElement(QByteArray ba);
    QString readString();
    quint32 readInt32();
    quint16 readInt16();
    quint8 readInt8();
    void writeString(QString string);
    void writeInt32(quint32 value);
    void writeInt16(quint16 value);
    void writeInt8(quint8 value);
    void append(QByteArray ba);
    quint32 type();
    quint32 subType();
    quint32 chatRoomIdentifier();
    QByteArray message();
    QByteArray data();
    operator QString() const;

private:
    void updateLength();
    quint32 _contentLength;
    quint32 _chatRoomIdentifier;
    quint32 _type;
    quint32 _subType;
    QByteArray _message;
    QByteArray _data;
    QDataStream * messageDataStream;


};

#endif // DATAELEMENT_H