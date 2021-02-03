#ifndef SERIALIZABLE_CONNECTOR_H
#define SERIALIZABLE_CONNECTOR_H

#include <QObject>
#include "serializer.h"
#include "connector.h"

class SerializableConnector : public QObject, public Connector
{
    Q_OBJECT

public:
    explicit SerializableConnector(QObject *parent = nullptr);

protected:
    Serializer *serializer;

Q_SIGNALS:
    void newResponse(uint8_t, const char*, uint8_t);
};

#endif // SERIALIZABLE_CONNECTOR_H
