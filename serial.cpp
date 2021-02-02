#include "serial.h"

Serial *Serial::_instance = new Serial;

Serial::Serial(QObject *parent) : SerializableConnector(parent),
                                  isWritable(false) {
    io = new QSerialPort;
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

void Serial::setSelectedPort(const QString &port) {
    this->port = "/dev/" + port;
}

void Serial::setSelectedBaudrate(const QString &rate) {
    baudrate = rate.toUInt();
}

void Serial::setSelectedDatabits(const QString &value) {
    switch (value.toUInt()) {
        case 5:
            dataBits = QSerialPort::Data5;
            break;
        case 6:
            dataBits = QSerialPort::Data6;
            break;
        case 7:
            dataBits = QSerialPort::Data7;
            break;
        case 8:
            dataBits = QSerialPort::Data8;
            break;
    }
}

void Serial::setSelectedParity(const QString &value) {
    if (value == "None") {
        parity = QSerialPort::NoParity;
    } else if (value == "Odd") {
        parity = QSerialPort::OddParity;
    } else if (value == "Even") {
        parity = QSerialPort::EvenParity;
    }
}

void Serial::setSelectedStopbits(const QString &value) {
    if (value == "1") {
        stopBits = QSerialPort::OneStop;
    } else if (value == "1.5") {
        stopBits = QSerialPort::OneAndHalfStop;
    } else if (value == "2") {
        stopBits = QSerialPort::TwoStop;
    }
}

bool Serial::Connect() {
    if (port.isEmpty() || baudrate == 0 || dataBits == 0 || parity == 0 || stopBits == 0) {
        return false;
    }
    io->setPortName(port);
    io->setBaudRate(baudrate);
    io->setDataBits(dataBits);
    io->setParity(parity);
    io->setStopBits(stopBits);
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

void Serial::SetConfig(const SerialInfo &config) {
    setSelectedPort(config.port);
    setSelectedBaudrate(config.rate);
    setSelectedDatabits(config.dataBits);
    setSelectedParity(config.parity);
    setSelectedStopbits(config.stopBits);
}
