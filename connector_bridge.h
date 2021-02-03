#ifndef CONNECTOR_BRIDGE_H
#define CONNECTOR_BRIDGE_H

#include <QObject>
#include <QMap>
#include <QBluetoothDeviceInfo>
#include "connector.h"
#include "connection_config.h"
#include "serializable_connector.h"

enum ProtocolType {
    SERIAL,
    BLE4
};

class ConnectorBridge : public QObject, public Connector {
Q_OBJECT

public:
    explicit ConnectorBridge(QObject *parent = nullptr);

public:
    void Scan() final;

    void StopScan() final;

    void SetConfig(ConnectionConfig *config) final;

    bool Connect() final;

    void Disconnect() final;

    bool Connected() final;

    void SendCommand(uint8_t command, const char *payload, uint8_t s) final;

    void SendCommand(uint8_t command) final;

    bool IsWritable() final;

public:
    static ConnectorBridge *instance() {
        static auto _instance = new ConnectorBridge;
        return _instance;
    };

private:
    bool connected;
    ProtocolType currentConnectionType;
    QMap<ProtocolType, ConnectionConfig *> configs;
    QMap<ProtocolType, SerializableConnector *> connectors;

Q_SIGNALS:

    void newSerialPort(QStringList);

    void newDeviceInfo(const QBluetoothDeviceInfo &info);

    void scanFinished();

    void newResponse(uint8_t, const char*, uint8_t);
};

#endif // CONNECTOR_BRIDGE_H
