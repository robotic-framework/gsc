#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <QObject>
#include <QString>
#include "stdint.h"
#include "protocol.h"

class Serializer: public QObject
{
    Q_OBJECT

public:
    Serializer();

public:
    static QString byteArrayToHexStr(const char *payload, int size);
    char* Serialize(uint8_t command, const char *payload, uint8_t s, uint8_t *size);
    char* Serialize(uint8_t command, uint8_t *size);
    void Decode(QByteArray payload);

private:
    char* _write(uint8_t *size);
    void _serialize(const char a);
    void _headWrite(uint8_t command, uint8_t s);
    void _tailWrite();
    void _requestWrite(const char *payload, uint8_t s);

private:
    char rxBuffer[INPUT_BUFFER_SIZE];
    uint8_t rxBufferIndex = 0;
    char txBuffer[TX_BUFFER_SIZE];
    uint8_t txBufferHead = 0;
    uint8_t state = IDLE;
    uint8_t command = 0;
    uint8_t dataSize = 0;
    uint8_t dataOffset = 0;
    uint8_t checksum = 0;

signals:
    void commandDecoded(uint8_t, const char*, uint8_t);
};

#endif // SERIALIZER_H
