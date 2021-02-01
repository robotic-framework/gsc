#ifndef CONNECTOR_BRIDGE_H
#define CONNECTOR_BRIDGE_H

#include <QObject>
#include "connector.h"

class ConnectorBridge : public QObject, public Connector {
Q_OBJECT

public:
    explicit ConnectorBridge(QObject *parent = nullptr);

public:
    void Scan() final;

    void StopScan() final;

    bool Connect() final;

    void Disconnect() final;

    bool Connected() final;

    void SendCommand(uint8_t command, const char *payload, uint8_t s) final;

    void SendCommand(uint8_t command) final;

    bool IsWritable() final;

public:
    static ConnectorBridge *instance() { return _instance; };

private:
    static ConnectorBridge *_instance;
};

#endif // CONNECTOR_BRIDGE_H
