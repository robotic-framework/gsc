#ifndef SERIAL_H
#define SERIAL_H

#include <QObject>
#include <QSerialPort>
#include <QStringList>
#include <QSerialPortInfo>
#include "protocol.h"
#include "serializable_connector.h"
#include "connection_config.h"

using namespace std;

class SerialInfo : public ConnectionConfig {

public:
    QString port;
    QString rate;
    QString dataBits;
    QString parity;
    QString stopBits;

    QString toString() const override {
        return QString("%1 [%2] %3-%4-%5").arg(port, rate, dataBits, parity, stopBits);
    };
};

class Serial : public SerializableConnector {
Q_OBJECT

private:
    explicit Serial(QObject *parent = nullptr);

public:
    static Serial *instance();

    void SetConfig(ConnectionConfig *config) final;

    void Scan() final;

    void StopScan() final;

    bool Connect() final;

    void Disconnect() final;

    bool Connected() final;

    void SendCommand(uint8_t command, const char *payload, uint8_t s) final;

    void SendCommand(uint8_t command) final;

    bool IsWritable() final;

protected:
    void timerEvent(QTimerEvent *event) override;

private slots:

    void onRead();

private:
    void setSelectedPort(const QString &port);

    void setSelectedBaudrate(const QString &rate);

    void setSelectedDatabits(const QString &value);

    void setSelectedParity(const QString &value);

    void setSelectedStopbits(const QString &value);

private:
    static Serial *_instance;
    QString port;
    uint baudrate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
    bool isWritable;
    uint timerId;

    QSerialPort *io;

signals:

    void newSerialPort(QStringList);
};

#endif // SERIAL_H
