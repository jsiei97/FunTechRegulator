#ifndef  __REGUL_H
#define  __REGUL_H

#include "Sensor.h"
#include "PID_v1.h"
#include "Output.h"

class Regul
{
    private:
        QString name;

        Sensor *sensor;
        Output *out;

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
};

#endif  // __REGUL_H
