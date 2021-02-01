#include "serializer.h"

Serializer::Serializer()
{

}

void Serializer::Decode(QByteArray payload)
{
    uchar *ptr = (uchar *)payload.constData();
    for (int i = 0; i < payload.size(); i++)
    {
        if (state == IDLE)
        {
            if (*ptr == '$') state = HEADER_START;
        }
        else if (state == HEADER_START)
        {
            state = (*ptr == 'M') ? HEADER_M : IDLE;
        }
        else if (state == HEADER_M)
        {
            state = (*ptr == '>') ? HEADER_ARROW : IDLE;
        }
        else if (state == HEADER_ARROW)
        {
            // size
            if (*ptr > INPUT_BUFFER_SIZE)
            {
                state = IDLE;
                continue;
            }

            dataSize = *ptr;
            checksum = dataSize;
            dataOffset = 0;
            state = HEADER_SIZE;
        }
        else if (state == HEADER_SIZE)
        {
            command = *ptr;
            checksum ^= command;
            state = HEADER_CMD;
        }
        else if (state == HEADER_CMD)
        {
            if (dataOffset < dataSize)
            {
                checksum ^= *ptr;
                rxBuffer[dataOffset++] = *ptr;
            }
            else
            {
                if (checksum == *ptr)
                {
                    emit commandDecoded(command, rxBuffer, dataSize);
                }
                state = IDLE;
            }
        }
        ptr++;
    }
}

char* Serializer::Serialize(uint8_t command, const char *payload, uint8_t s, uint8_t *size)
{
    _headWrite(command, s);
    _requestWrite(payload, s);
    _tailWrite();
    return _write(size);
}

char* Serializer::Serialize(uint8_t command, uint8_t *size)
{
    return Serialize(command, nullptr, 0, size);
}

char* Serializer::_write(uint8_t *size)
{
//    qDebug("serial send: %s", byteArrayToHexStr(txBuffer, txBufferHead).toStdString().c_str());
    *size = txBufferHead;
    txBufferHead = 0;
    return txBuffer;
}

void Serializer::_serialize(const char a)
{
    uint8_t t = txBufferHead;
    if (t >= TX_BUFFER_SIZE)
        return;
    txBuffer[t] = a;
    t++;
    txBufferHead = t;
    checksum ^= a;
}

void Serializer::_headWrite(uint8_t command, uint8_t s)
{
    _serialize('$');
    _serialize('M');
    _serialize('<');

    checksum = 0;
    _serialize(s);
    _serialize(command);
}

void Serializer::_tailWrite()
{
    _serialize(checksum);
}

void Serializer::_requestWrite(const char *payload, uint8_t s)
{
    while (s--)
    {
        _serialize(*payload++);
    }
}

QString Serializer::byteArrayToHexStr(const char *payload, int size)
{
    QByteArray data = QByteArray::fromRawData(payload, size);
    QString temp = "";
    QString hex = data.toHex();

    for (int i = 0; i < hex.length(); i = i + 2) {
        temp += hex.mid(i, 2) + " ";
    }

    return temp.trimmed().toUpper();
}
