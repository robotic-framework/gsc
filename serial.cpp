#include "serial.h"

Serial *Serial::_instance = new Serial;

Serial::Serial(QObject *parent) : SerializableConnector(parent),
                                  isWritable(false) {
    io = new QSerialPort;
    baudrate = 115200;
    databits = QSerialPort::Data8;
    parity = QSerialPort::NoParity;
    stopbits = QSerialPort::OneStop;
    connect(serializer, &Serializer::commandDecoded, this, [this](uint8_t command, const char *payload, uint8_t s) {
        emit newResponse(command, payload, s);
    });
}

Serial *Serial::instance() {
    return _instance;
}

void Serial::Scan() {
    QStringList newPortStringList;
    //搜索串口
            foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
#if DEBUG_INFOR_EN
            qDebug() << "Name        : " << info.portName();
            qDebug() << "Description : " << info.description();
            qDebug() << "Manufacturer: " << info.manufacturer();
#endif
            newPortStringList += info.portName();
        }

    emit newSerialPort(newPortStringList);
}

void Serial::StopScan() {

}

void Serial::SetSelectedPort(const QString &port) {
    this->port = "/dev/" + port;
}

void Serial::SetSelectedBaudrate(const QString &rate) {
    baudrate = rate.toUInt();
}

void Serial::SetSelectedDatabits(const QString &value) {
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

void Serial::SetSelectedParity(const QString &value) {
    if (value == "None") {
        parity = QSerialPort::NoParity;
    } else if (value == "Odd") {
        parity = QSerialPort::OddParity;
    } else if (value == "Even") {
        parity = QSerialPort::EvenParity;
    }
}

void Serial::SetSelectedStopbits(const QString &value) {
    if (value == "1") {
        stopbits = QSerialPort::OneStop;
    } else if (value == "1.5") {
        stopbits = QSerialPort::OneAndHalfStop;
    } else if (value == "2") {
        stopbits = QSerialPort::TwoStop;
    }
}

QString Serial::GetSelectedPort() {
    return port;
}

uint Serial::GetSelectedBaudrate() {
    return baudrate;
}

QSerialPort::DataBits Serial::GetSelectedDatabits() {
    return databits;
}

QSerialPort::Parity Serial::GetSelectedParity() {
    return parity;
}

QSerialPort::StopBits Serial::GetSelectedStopbits() {
    return stopbits;
}

bool Serial::Connect() {
    io->setPortName(port);
    io->setBaudRate(baudrate);
    io->setDataBits(databits);
    io->setParity(parity);
    io->setStopBits(stopbits);
    io->setFlowControl(QSerialPort::NoFlowControl);

    if (io->open(QIODevice::ReadWrite)) {
        connect(io, SIGNAL(readyRead()), this, SLOT(onRead()));
        // 连接后等待5秒变为可写状态
        timerId = startTimer(5000);
        return true;
    }

    return false;
}

void Serial::Disconnect() {
    if (io->isOpen()) {
        disconnect(io, SIGNAL(readyRead()), this, SLOT(onRead()));
        io->clear();
        io->close();
    }

    if (isWritable) {
        isWritable = false;
        if (timerId) {
            killTimer(timerId);
        }
    }
}

void Serial::timerEvent(QTimerEvent *event) {
    isWritable = true;
    killTimer(timerId);
    timerId = 0;
}

bool Serial::IsWritable() {
    return isWritable;
}

bool Serial::Connected() {
    return io->isOpen();
}

void Serial::SendCommand(uint8_t command, const char *payload, uint8_t s) {
    if (!io->isOpen()) {
        return;
    }
    if (!isWritable) {
        return;
    }
    char *result = nullptr;
    uint8_t size = 0;
    result = serializer->Serialize(command, payload, s, &size);

    io->write(result, size);
}

void Serial::SendCommand(uint8_t command) {
    SendCommand(command, nullptr, 0);
}

void Serial::onRead() {
    QByteArray payload = io->readAll();
    serializer->Decode(payload);
}
