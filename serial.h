#ifndef SERIAL_H
#define SERIAL_H

#include <QObject>
#include <QSerialPort>
#include <QStringList>
#include <QSerialPortInfo>
#include "protocol.h"
#include "serializable_connector.h"

using namespace std;

class Serial : public SerializableConnector
{
    Q_OBJECT

private:
    Serial(QObject *parent = nullptr);

public:
    static Serial* instance();
    void SetSelectedPort(const QString &port);
    void SetSelectedBaudrate(const QString &rate);
    void SetSelectedDatabits(const QString &value);
    void SetSelectedParity(const QString &value);
    void SetSelectedStopbits(const QString &value);
    QString GetSelectedPort();
    uint GetSelectedBaudrate();
    QSerialPort::DataBits GetSelectedDatabits();
    QSerialPort::Parity GetSelectedParity();
    QSerialPort::StopBits GetSelectedStopbits();

    void Scan() final;
    void StopScan() final;
    bool Connect() final;
    void Disconnect() final;
    bool Connected() final;
    void SendCommand(uint8_t command, const char *payload, uint8_t s) final;
    void SendCommand(uint8_t command) final;
    bool IsWritable() final;

protected:
    void timerEvent( QTimerEvent *event );

private slots:
    void onRead();


private:
    static Serial* _instance;
    QString port;
    uint baudrate;
    QSerialPort::DataBits databits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopbits;
    bool isWritable;
    uint timerId;

    QSerialPort *io;

signals:
    void newSerialPort(QStringList);
};

#endif // SERIAL_H
