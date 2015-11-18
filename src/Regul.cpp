
#include <QDebug>
#include <QString>

#include "Regul.h"
#include "Sensor.h"
#include "Debug.h"
#include "PID_v1.h"

#include "Output.h"

#include "MosqConnect.h"

Regul::Regul()
{
    sensor = NULL;
    out    = NULL;
    mqtt   = NULL;

    name = "Default name";

    input    = 5;
    output   = 0;
    setpoint = 20;

    pid = new PID(input, output, setpoint, 2, 5, 1, DIRECT);
}

int Regul::start()
{
    if(NULL==sensor)
        return 10;

    //if(NULL==output) return 11;

    //check pid

    //start the pid
    pid->setMode(AUTOMATIC);
    return 0;
}

bool Regul::compute()
{
    double tmpvalue;
    if(sensor->getValue(&tmpvalue))
    {
        input = tmpvalue;

        double tmpout;
        bool res = pid->compute(input, &tmpout);
        if(res)
        {
            out->putValue(tmpout);
            if(NULL != mqtt)
            {
                //value=%d.%02d ; setpoint=%d.%02d ; output=%03d%%"
                QString str = QString("value=%1 ; setpoint=%2 ; output=%3\%")
                    .arg(input,    0, 'f', 2)
                    .arg(setpoint, 0, 'f', 2)
                    .arg(tmpout,   0, 'f', 0);

                myOut() << mqttPublishTopic << str;
                mqtt->pub(mqttPublishTopic, str);
            }
        }
        return res;
    }

    myErr() << "Problem with sensor getValue";
    return false;
}


void Regul::setName(QString name)
{
    this->name = name;
}

void Regul::setMQTTpublish(QString topic, MosqConnect *mqtt)
{
    mqttPublishTopic = topic;
    this->mqtt = mqtt;
}

void Regul::setMQTTsubscribe(QString topic, MosqConnect *mqtt)
{
    mqttSubscribeTopic = topic;
    this->mqtt = mqtt;
}

void Regul::setInput(Sensor *s)
{
    sensor = s;
}

void Regul::setOutput(Output *out)
{
    this->out = out;
}
void Regul::print()
{
    /*
       myOut() << this->name << ":" << this->out;
       if(sensor != NULL)
       {
       sensor->print();
       }
       */

    //myOut() << this->name << "Status" << input << setpoint << output;
}
