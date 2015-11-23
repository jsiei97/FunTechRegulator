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

        Sensor *sensor;
        PID *pid;
        Output *out;

        MosqConnect *mqtt;
        QString mqttPublishTopic;
        QString mqttSubscribeTopic;

        double input;     ///< The value
        double sentInput; ///< Value sent
        double diffInput; ///< How much diff to allow before we send

        double setpoint;
        double sentSetpoint;
        //double diffSetpoint;

        double output;
        double sentOutput;
        double diffOutput;

        QElapsedTimer sendTimer;

    public:
        Regul();
        int start();
        bool compute();

        QString timeToSend();

        void print();

        void setName(QString name);
        void setInput(Sensor *in);
        void setOutput(Output *out);

        void setMQTTpublish  (QString topic, MosqConnect *mqtt);
        void setMQTTsubscribe(QString topic, MosqConnect *mqtt);

        void setSetpoint(QString value);
        void setKp(QString value);
        void setKi(QString value);
        void setKd(QString value);
};

#endif  // __REGUL_H
