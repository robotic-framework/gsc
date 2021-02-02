#include "connector_bridge.h"

ConnectorBridge::ConnectorBridge(QObject *parent) : QObject(parent)
{

}

void ConnectorBridge::Scan() {

}

void ConnectorBridge::StopScan() {

}

bool ConnectorBridge::Connect() {
    return false;
}

void ConnectorBridge::Disconnect() {

}

bool ConnectorBridge::Connected() {
    return false;
}

void ConnectorBridge::SendCommand(uint8_t command, const char *payload, uint8_t s) {

}

void ConnectorBridge::SendCommand(uint8_t command) {

}

bool ConnectorBridge::IsWritable() {
    return false;
}


