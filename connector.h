#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "stdint.h"

class Connector {

public:

    virtual void Scan() = 0;

    virtual void StopScan() = 0;

    virtual bool Connect() = 0;

    virtual void Disconnect() = 0;

    virtual bool Connected() = 0;

    virtual void SendCommand(uint8_t command, const char *payload, uint8_t s) = 0;

    virtual void SendCommand(uint8_t command) = 0;

    virtual bool IsWritable() = 0;
};

#endif // CONNECTOR_H
