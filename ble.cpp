#include "ble.h"

BLE* BLE::_instance = new BLE;

BLE::BLE(QObject *parent) : QObject(parent)
{
    serializer = new Serializer;
    connect(serializer, &Serializer::commandDecoded, this, [this](uint8_t command, const char* payload, uint8_t s)
    {
        emit newResponse(command, payload, s);
    });

    bleAgent = new QBluetoothDeviceDiscoveryAgent;
    bleAgent->setLowEnergyDiscoveryTimeout(10000); // 10s
    connect(bleAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo &)), this, SLOT(onNewDeviceAdd(const QBluetoothDeviceInfo &)));
    connect(bleAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, [this]()
    {
        emit scanFinished();
    });
}

BLE* BLE::instance()
{
    return _instance;
}

void BLE::Scan()
{
    bleAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void BLE::StopScan()
{
    bleAgent->stop();
}

void BLE::Connect(const QBluetoothDeviceInfo &info)
{
    bleController = new QLowEnergyController(info);
    connect(bleController, SIGNAL(connected()), this, SLOT(onServiceConnected()));
    connect(bleController, SIGNAL(serviceDiscovered(const QBluetoothUuid &)), this, SLOT(onServiceDiscovered(const QBluetoothUuid &)));
    connect(bleController, SIGNAL(discoveryFinished()), this, SLOT(onServiceDiscoverFinished()));
    bleController->connectToDevice();
}

void BLE::Disconnect()
{
    if (bleController != nullptr)
    {
        bleController->disconnectFromDevice();
        disconnect(bleController, SIGNAL(connected()), this, SLOT(onServiceConnected()));
        disconnect(bleController, SIGNAL(serviceDiscovered(const QBluetoothUuid &)), this, SLOT(onServiceDiscovered(const QBluetoothUuid &)));
        disconnect(bleController, SIGNAL(discoveryFinished()), this, SLOT(onServiceDiscoverFinished()));
        bleController = nullptr;

        disconnect(bleService, SIGNAL(stateChanged(QLowEnergyService::ServiceState)), this, SLOT(onServiceStateChanged(QLowEnergyService::ServiceState)));
        disconnect(bleService, SIGNAL(characteristicChanged(const QLowEnergyCharacteristic &, const QByteArray &)), this, SLOT(onCharacteristicChanged(const QLowEnergyCharacteristic &, const QByteArray &)));
        disconnect(bleService, SIGNAL(characteristicRead(const QLowEnergyCharacteristic &, const QByteArray &)), this, SLOT(onCharacteristicRead(const QLowEnergyCharacteristic &, const QByteArray &)));
        disconnect(bleService, SIGNAL(characteristicWritten(const QLowEnergyCharacteristic &, const QByteArray &)), this, SLOT(onCharacteristicWritten(const QLowEnergyCharacteristic &, const QByteArray &)));
        bleService = nullptr;
    }
}

void BLE::SendCommand(uint8_t command, const char *payload, uint8_t s)
{
    if (!IsWritable())
    {
        return;
    }

    char *result = nullptr;
    uint8_t size = 0;
    result = serializer->Serialize(command, payload, s, &size);

    QByteArray data = QByteArray(result, size);
    bleService->writeCharacteristic(targetChar, data, QLowEnergyService::WriteWithoutResponse);
}

void BLE::SendCommand(uint8_t command)
{
    SendCommand(command, nullptr, 0);
}

bool BLE::IsWritable()
{
    return (bleService && targetChar.uuid().toString() != "{00000000-0000-0000-0000-000000000000}");
}

void BLE::onNewDeviceAdd(const QBluetoothDeviceInfo &info)
{
    if (!(info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) || info.name().isEmpty())
    {
        return;
    }

    for (int i=0; i < deviceList.count(); i++)
    {
        if (deviceList[i].deviceUuid().toString() == info.deviceUuid().toString())
        {
            return;
        }
    }

    deviceList.append(info);
    emit newDeviceAdd(info);
}

void BLE::onServiceConnected()
{
    emit serviceConnected();
    bleController->discoverServices();
}

void BLE::onServiceDiscovered(const QBluetoothUuid &newService)
{
    emit serviceDiscovered(newService);
}

void BLE::onServiceDiscoverFinished()
{
    emit serviceDiscoverFinished();
    QList<QBluetoothUuid> services = bleController->services();
    for (int i = 0; i < services.count(); i++)
    {
        if (services[i].toString() == SERVICE_UUID)
        {
            bleService = bleController->createServiceObject(services[i]);
            if (!bleService)
            {
                return;
            }

            connect(bleService, SIGNAL(stateChanged(QLowEnergyService::ServiceState)), this, SLOT(onServiceStateChanged(QLowEnergyService::ServiceState)));
            connect(bleService, SIGNAL(characteristicChanged(const QLowEnergyCharacteristic &, const QByteArray &)), this, SLOT(onCharacteristicChanged(const QLowEnergyCharacteristic &, const QByteArray &)));
            connect(bleService, SIGNAL(characteristicRead(const QLowEnergyCharacteristic &, const QByteArray &)), this, SLOT(onCharacteristicRead(const QLowEnergyCharacteristic &, const QByteArray &)));
            connect(bleService, SIGNAL(characteristicWritten(const QLowEnergyCharacteristic &, const QByteArray &)), this, SLOT(onCharacteristicWritten(const QLowEnergyCharacteristic &, const QByteArray &)));

            if (bleService->state() == QLowEnergyService::DiscoveryRequired)
            {
                bleService->discoverDetails();
            }
            else
            {
                searchCharacteristic();
            }
            return;
        }
    }
}

void BLE::onServiceStateChanged(QLowEnergyService::ServiceState state)
{
    emit serviceStateChanged(state);
    if (state == QLowEnergyService::ServiceDiscovered)
    {
        searchCharacteristic();
    }
}

void BLE::onCharacteristicChanged(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    emit characteristicChanged(c, value);
    qDebug("change: %s", value.toStdString().c_str());

    serializer->Decode(value);
}

void BLE::onCharacteristicRead(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    emit characteristicRead(c, value);
    qDebug("read: %s", value.toStdString().c_str());
}

void BLE::onCharacteristicWritten(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    emit characteristicWritten(c, value);
    qDebug("write: %s", value.toHex().toStdString().c_str());
}

void BLE::searchCharacteristic()
{
    if (!bleService) return;

    QList<QLowEnergyCharacteristic> chars = bleService->characteristics();
    int count = chars.count();

    for (int i = 0; i < count; i++)
    {
        if (chars[i].uuid().toString() == CHAR_UUID)
        {
            targetChar = chars[i];
        }
    }
}
