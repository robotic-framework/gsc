#ifndef SERIALIZABLE_CONNECTOR_H
#define SERIALIZABLE_CONNECTOR_H

#include <QObject>
#include "serializer.h"
#include "connector.h"

class SerializableConnector : public QObject, public Connector
{
    Q_OBJECT

public:
    SerializableConnector(QObject *parent);

protected:
    Serializer *serializer;

signals:
    void newResponse(uint8_t, const char*, uint8_t);
};

#endif // SERIALIZABLE_CONNECTOR_H
