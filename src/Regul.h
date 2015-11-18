#ifndef  __REGUL_H
#define  __REGUL_H

#include "Sensor.h"
#include "PID_v1.h"
#include "Output.h"
#include "MosqConnect.h"

class Regul
{
    private:
        QString name;
        QString mqttPublishTopic;
        QString mqttSubscribeTopic;

        Sensor *sensor;
        Output *out;
        MosqConnect *mqtt;

        double setpoint, input, output;
        PID *pid;

    public:
        Regul();
        int start();
        bool compute();

        void print();

        void setName(QString name);
        void setInput(Sensor *in);
        void setOutput(Output *out);

        void setMQTTpublish  (QString topic, MosqConnect *mqtt);
        void setMQTTsubscribe(QString topic, MosqConnect *mqtt);
};

#endif  // __REGUL_H
