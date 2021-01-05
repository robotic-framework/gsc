#include "serial.h"

Serial* Serial::_instance = new Serial;

Serial::Serial(): isWritable(false)
{
    io = new QSerialPort;
    baudrate = 115200;
    databits = QSerialPort::Data8;
    parity = QSerialPort::NoParity;
    stopbits = QSerialPort::OneStop;
}

Serial* Serial::instance()
{
    return _instance;
}

void Serial::ScanPorts()
{
    QStringList newPortStringList;
    //搜索串口
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
      #if DEBUG_INFOR_EN
        qDebug() << "Name        : " << info.portName();
        qDebug() << "Description : " << info.description();
        qDebug() << "Manufacturer: " << info.manufacturer();
      #endif
      newPortStringList += info.portName();
    }

    emit newSerialPort(newPortStringList);
}

void Serial::SetSelectedPort(const QString &port)
{
    this->port = "/dev/" + port;
}

void Serial::SetSelectedBaudrate(const QString &rate)
{
    baudrate = rate.toUInt();
}

void Serial::SetSelectedDatabits(const QString &value)
{
    switch (value.toUInt()) {
    case 5:
        databits = QSerialPort::Data5;
        break;
    case 6:
        databits = QSerialPort::Data6;
        break;
    case 7:
        databits = QSerialPort::Data7;
        break;
    case 8:
        databits = QSerialPort::Data8;
        break;
    }
}

void Serial::SetSelectedParity(const QString &value)
{
    if (value == "None")
    {
        parity = QSerialPort::NoParity;
    }
    else if (value == "Odd")
    {
        parity = QSerialPort::OddParity;
    }
    else if (value == "Even")
    {
        parity = QSerialPort::EvenParity;
    }
}

void Serial::SetSelectedStopbits(const QString &value)
{
    if (value == "1")
    {
        stopbits = QSerialPort::OneStop;
    }
    else if (value == "1.5")
    {
        stopbits = QSerialPort::OneAndHalfStop;
    }
    else if (value == "2")
    {
        stopbits = QSerialPort::TwoStop;
    }
}

QString Serial::GetSelectedPort()
{
    return port;
}

uint Serial::GetSelectedBaudrate()
{
    return baudrate;
}

QSerialPort::DataBits Serial::GetSelectedDatabits()
{
    return databits;
}

QSerialPort::Parity Serial::GetSelectedParity()
{
    return parity;
}

QSerialPort::StopBits Serial::GetSelectedStopbits()
{
    return stopbits;
}

bool Serial::Connect()
{
    io->setPortName(port);
    io->setBaudRate(baudrate);
    io->setDataBits(databits);
    io->setParity(parity);
    io->setStopBits(stopbits);
    io->setFlowControl(QSerialPort::NoFlowControl);

    if (io->open(QIODevice::ReadWrite))
    {
        connect(io, SIGNAL(readyRead()), this, SLOT(onRead()));
        // 连接后等待5秒变为可写状态
        timerId = startTimer(5000);
        return true;
    }

    return false;
}

void Serial::Disconnect()
{
    if (io->isOpen())
    {
        disconnect(io, SIGNAL(readyRead()), this, SLOT(onRead()));
        io->clear();
        io->close();
    }

    if (isWritable)
    {
        isWritable = false;
        if (timerId)
        {
            killTimer(timerId);
        }
    }
}

void Serial::timerEvent( QTimerEvent *event )
{
    isWritable = true;
    killTimer(timerId);
    timerId = 0;
}

bool Serial::IsWritable()
{
    return isWritable;
}

bool Serial::Connected()
{
    return io->isOpen();
}

void Serial::_write()
{
    io->write(txBuffer, txBufferHead);
    txBufferHead = 0;
}

void Serial::_serialize(const char a)
{
    uint8_t t = txBufferHead;
    if (t >= TX_BUFFER_SIZE)
        _write();
    txBuffer[t] = a;
    t++;
    txBufferHead = t;
    checksum ^= a;
}

void Serial::_headWrite(uint8_t command, uint8_t s)
{
    _serialize('$');
    _serialize('M');
    _serialize('<');

    checksum = 0;
    _serialize(s);
    _serialize(command);
}

void Serial::_tailWrite()
{
    _serialize(checksum);
}

void Serial::_requestWrite(const char *payload, uint8_t s)
{
    while (s--)
    {
        _serialize(*payload++);
    }
}

void Serial::SendCommand(uint8_t command, const char *payload, uint8_t s)
{
    if (!io->isOpen())
    {
        return;
    }
    if (!isWritable)
    {
        return;
    }
    _headWrite(command, s);
    _requestWrite(payload, s);
    _tailWrite();
    _write();
}

void Serial::SendCommand(uint8_t command)
{
    SendCommand(command, nullptr, 0);
}

void Serial::onRead()
{
    QByteArray payload = io->readAll();
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
                    emit newResponse(command, rxBuffer, dataSize);
                }
                state = IDLE;
            }
        }
        ptr++;
    }
}
