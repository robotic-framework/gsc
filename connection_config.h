#ifndef CONNECTIONCONFIG_H
#define CONNECTIONCONFIG_H

#include <QString>

class ConnectionConfig
{
public:
    virtual QString toString() const = 0;
};

#endif // CONNECTIONCONFIG_H
