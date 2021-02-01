#include "serializable_connector.h"

SerializableConnector::SerializableConnector(QObject *parent) : QObject(parent)
{
    serializer = new Serializer;
}
