#include "connector_bridge.h"
#include "serial.h"
#include "ble.h"

ConnectorBridge::ConnectorBridge(QObject *parent) : QObject(parent),
                                                    currentConnectionType(SERIAL),
                                                    connected(false) {

    auto serial = Serial::instance();
    connectors.insert(SERIAL, serial);
    connect(serial, &Serial::newSerialPort, this, [this](QStringList ports) {
        emit newSerialPort(ports);
    });
    connect(serial, &Serial::newResponse, this, [this](uint8_t command, const char* payload, uint8_t s) {
        emit newResponse(command, payload, s);
    });
    auto ble = BLE::instance();
    connectors.insert(BLE4, BLE::instance());
    connect(ble, &BLE::newDeviceAdd, this, [this](const QBluetoothDeviceInfo &device) {
        emit newDeviceInfo(device);
    });
    connect(ble, &BLE::scanFinished, this, [this]() {
        emit scanFinished();
    });
    connect(ble, &Serial::newResponse, this, [this](uint8_t command, const char* payload, uint8_t s) {
        emit newResponse(command, payload, s);
    });
}

void ConnectorBridge::Scan() {
    connectors[currentConnectionType]->Scan();
}

void ConnectorBridge::StopScan() {
    connectors[currentConnectionType]->StopScan();
}

void ConnectorBridge::SetConfig(ConnectionConfig *config) {
    try {
        auto serialConf = dynamic_cast<const SerialInfo *>(config);
        if (serialConf != nullptr) {
            configs[SERIAL] = config;
            if (!connected) {
                currentConnectionType = SERIAL;
            }
            return;
        }

        const auto bleConf = dynamic_cast<const BLEInfo *>(config);
        if (bleConf != nullptr) {
            configs[BLE4] = config;
            if (!connected) {
                currentConnectionType = BLE4;
            }
            return;
        }

        throw std::runtime_error("cannot cast ConnectionConfig into SerialInfo or BLEInfo");
    } catch (std::exception &e) {
        throw e;
    }
}

bool ConnectorBridge::Connect() {
    if (!configs.contains(currentConnectionType)) {
        return false;
    }
    connectors[currentConnectionType]->SetConfig(configs[currentConnectionType]);
    connected = connectors[currentConnectionType]->Connect();
    return connected;
}

void ConnectorBridge::Disconnect() {
    connectors[currentConnectionType]->Disconnect();
    connected = false;
}

bool ConnectorBridge::Connected() {
    return connectors[currentConnectionType]->Connected();
}

void ConnectorBridge::SendCommand(uint8_t command, const char *payload, uint8_t s) {
    if (!connected) {
        return;
    }
    connectors[currentConnectionType]->SendCommand(command, payload, s);
}

void ConnectorBridge::SendCommand(uint8_t command) {
    connectors[currentConnectionType]->SendCommand(command);
}

bool ConnectorBridge::IsWritable() {
    if (!connected) {
        return false;
    }
    return connectors[currentConnectionType]->IsWritable();
}
