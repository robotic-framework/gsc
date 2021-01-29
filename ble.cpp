#include "ble.h"

BLE* BLE::_instance = new BLE;

BLE::BLE(QObject *parent) : QObject(parent)
{
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
    _headWrite(command, s);
    _requestWrite(payload, s);
    _tailWrite();
    _write();

    bleService->readCharacteristic(targetChar);
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

    QByteArray payload = value;
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

void BLE::_write()
{
    QByteArray data = QByteArray(txBuffer, txBufferHead);
    txBufferHead = 0;
    bleService->writeCharacteristic(targetChar, data, QLowEnergyService::WriteWithoutResponse);
}

void BLE::_serialize(const char a)
{
    uint8_t t = txBufferHead;
    if (t >= TX_BUFFER_SIZE)
        _write();
    txBuffer[t] = a;
    t++;
    txBufferHead = t;
    checksum ^= a;
}

void BLE::_headWrite(uint8_t command, uint8_t s)
{
    _serialize('$');
    _serialize('M');
    _serialize('<');

    checksum = 0;
    _serialize(s);
    _serialize(command);
}

void BLE::_tailWrite()
{
    _serialize(checksum);
}

void BLE::_requestWrite(const char *payload, uint8_t s)
{
    while (s--)
    {
        _serialize(*payload++);
    }
}
