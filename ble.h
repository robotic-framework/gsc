#ifndef BLE_H
#define BLE_H

#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include "protocol.h"

#define SERVICE_UUID "{0000ffe0-0000-1000-8000-00805f9b34fb}"
#define CHAR_UUID "{0000ffe1-0000-1000-8000-00805f9b34fb}"

class BLE : public QObject
{
    Q_OBJECT
public:
    explicit BLE(QObject *parent = nullptr);

public:
    static BLE* instance();
    void Scan();
    void StopScan();
    void Connect(const QBluetoothDeviceInfo &info);
    void Disconnect();
    void SendCommand(uint8_t command, const char *payload, uint8_t s);
    void SendCommand(uint8_t command);
    bool IsWritable();

private:
    static BLE *_instance;
    QBluetoothDeviceDiscoveryAgent *bleAgent;
    QList<QBluetoothDeviceInfo> deviceList;
    QLowEnergyController *bleController;
    QLowEnergyService *bleService;
    QLowEnergyCharacteristic targetChar;

    char rxBuffer[INPUT_BUFFER_SIZE];
    uint8_t rxBufferIndex;
    char txBuffer[TX_BUFFER_SIZE];
    uint8_t txBufferHead;
    uint8_t state = IDLE;
    uint8_t command = 0;
    uint8_t dataSize = 0;
    uint8_t dataOffset = 0;
    uint8_t checksum = 0;

    void searchCharacteristic();

    void _write();
    void _serialize(const char a);
    void _headWrite(uint8_t command, uint8_t s);
    void _tailWrite();
    void _requestWrite(const char *payload, uint8_t s);

private slots:
    void onNewDeviceAdd(const QBluetoothDeviceInfo &info);
    void onServiceDiscovered(const QBluetoothUuid &newService);
    void onServiceDiscoverFinished();
    void onServiceConnected();
    void onServiceStateChanged(QLowEnergyService::ServiceState);
    void onCharacteristicChanged(const QLowEnergyCharacteristic &c, const QByteArray &value);
    void onCharacteristicRead(const QLowEnergyCharacteristic &c, const QByteArray &value);
    void onCharacteristicWritten(const QLowEnergyCharacteristic &c, const QByteArray &value);

signals:
    void scanFinished();
    void newDeviceAdd(const QBluetoothDeviceInfo &info);
    void serviceConnected();
    void serviceDiscovered(const QBluetoothUuid &newService);
    void serviceDiscoverFinished();
    void serviceStateChanged(QLowEnergyService::ServiceState);
    void characteristicChanged(const QLowEnergyCharacteristic &, const QByteArray &);
    void characteristicRead(const QLowEnergyCharacteristic &, const QByteArray &);
    void characteristicWritten(const QLowEnergyCharacteristic &, const QByteArray &);
    void newResponse(uint8_t, const char*, uint8_t);
};

#endif // BLE_H