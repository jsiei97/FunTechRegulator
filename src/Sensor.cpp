
#include <QDebug>
#include <QString>

#include "Sensor.h"
#include "SysfsOW.h"
#include "Debug.h"

Sensor::Sensor()
{
    type = SENSOR_NONE;
}

void Sensor::setName(QString name)
{
    this->name = name;
}

void Sensor::setOut(QString out)
{
    this->out = out;
}

void Sensor::setOneWire(QString address)
{
    type = SENSOR_SYSFS_ONEWIRE;
    this->address = address;
}

bool Sensor::getValue(double *value)
{
    switch ( type )
    {
        case SENSOR_SYSFS_ONEWIRE :
            return SysfsOW::getValue("28-000000b51bac", value);
            break;
        case SENSOR_NONE :
        default :
            // ???
            break;
    }

    return false;
}

void Sensor::print()
{
    myOut() << this->name << ":" << this->out;
}
