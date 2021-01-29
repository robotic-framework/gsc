#ifndef SERIAL_H
#define SERIAL_H

#include <QObject>
#include <QSerialPort>
#include <QStringList>
#include <QSerialPortInfo>
#include "protocol.h"
#include "serializer.h"

using namespace std;

class Serial : public QObject
{
    Q_OBJECT

public:
    static Serial* instance();
    void ScanPorts();
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
    bool Connect();
    void Disconnect();
    bool Connected();
    void SendCommand(uint8_t command, const char *payload, uint8_t s);
    void SendCommand(uint8_t command);
    bool IsWritable();

protected:
    void timerEvent( QTimerEvent *event );

private:
    Serial();

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
    Serializer *serializer;

signals:
    void newSerialPort(QStringList);
    void newResponse(uint8_t, const char*, uint8_t);
};

#endif // SERIAL_H
