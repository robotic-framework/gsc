#ifndef BLE_H
#define BLE_H

#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include "serializable_connector.h"
#include "protocol.h"

#define SERVICE_UUID "{0000ffe0-0000-1000-8000-00805f9b34fb}"
#define CHAR_UUID "{0000ffe1-0000-1000-8000-00805f9b34fb}"
#define INVALID_UUID "{00000000-0000-0000-0000-000000000000}"

class BLE : public SerializableConnector {
Q_OBJECT

private:
    explicit BLE(QObject *parent = nullptr);

public:
    static BLE *instance();

    void SetCurrentDevice(const QBluetoothDeviceInfo &info) { currentDevice = info; };

    void Scan() final;

    void StopScan() final;

    bool Connect() final;

    void Disconnect() final;

    bool Connected() final;

    void SendCommand(uint8_t command, const char *payload, uint8_t s) final;

    void SendCommand(uint8_t command) final;

    bool IsWritable() final;

private:
    static BLE *_instance;
    QBluetoothDeviceInfo currentDevice;
    QBluetoothDeviceDiscoveryAgent *bleAgent;
    QList<QBluetoothDeviceInfo> deviceList;
    QLowEnergyController *bleController;
    QLowEnergyService *bleService;
    QLowEnergyCharacteristic targetChar;

    void searchCharacteristic();

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
};

#endif // BLE_H
