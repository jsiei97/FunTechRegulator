#ifndef  __SENSOR_H
#define  __SENSOR_H

typedef enum ST
{
    SENSOR_NONE = 0,     ///< No configured sensor
    SENSOR_SYSFS_ONEWIRE ///< A OneWire sensor (i.e. DS18B20) connected on sysfs
} SensorType;

class Sensor
{
    private:
        SensorType type;

        QString name;
        QString out;
        QString address;

    public:
        Sensor();

        void setName(QString name);
        void setOut(QString out);
        void setOneWire(QString address);

        bool getValue(double *value);
        void print();
};

#endif  // __SENSOR_H
