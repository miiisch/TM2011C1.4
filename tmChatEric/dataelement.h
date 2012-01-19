#ifndef DATAELEMENT_H
#define DATAELEMENT_H

#include <QString>

typedef quint32 userId;

class DataElement
{
public:
    DataElement(quint32 chatRoomIdentifier, quint32 type, quint32 subType, quint32 sender, quint32 receiver);
    DataElement(QByteArray ba);
    QString readString(bool * valid = 0);
    quint32 readInt32(bool * valid = 0);
    quint16 readInt16(bool * valid = 0);
    quint8 readInt8(bool * valid = 0);
    void writeString(QString string);
    void writeInt32(quint32 value);
    void writeInt16(quint16 value);
    void writeInt8(quint8 value);
    void append(QByteArray ba);
    quint32 type() const;
    void setType(quint32 type);
    quint32 subType() const;
    void setSubType(quint32 subType);
    quint32 chatRoomIdentifier() const;
    quint32 sender() const;
    quint32 receiver() const;
    QByteArray message() const;
    QByteArray data();
    operator QString() const;

    QString rawData() const;
    bool rewind() const;

private:
    void updateLength();
    quint32 _contentLength;
    quint32 _chatRoomIdentifier;
    quint32 _type;
    quint32 _subType;
    quint32 _sender;
    quint32 _receiver;
    QByteArray _message;
    QByteArray _data;
    QDataStream * messageDataStream;


};

#endif // DATAELEMENT_H
