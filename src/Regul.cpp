
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

    input = 20;
    sentInput = 100;
    diffInput = 0.2;

    setpoint = 20;
    sentSetpoint = 100;
    //diffSetpoint;

    output     = 0;
    sentOutput = 0;
    diffOutput = 1;

    sendTimer.start();

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
    static unsigned sensorcheck = 0;
    double tmpvalue;
    if(sensor->getValue(&tmpvalue))
    {
        input = tmpvalue;
        sensorcheck = 0;

        double tmpout;
        bool res = pid->compute(input, &tmpout);
        if(res)
        {
            output = tmpout;
        }

        out->putValue(output);
        QString str = timeToSend();
        if(NULL != str && NULL != mqtt)
        {
            myOut() << mqttPublishTopic << str;
            mqtt->pub(mqttPublishTopic, str);
        }
        return res;
    }
    else
    {
        sensorcheck++;
        if(sensorcheck == 30)
        {
            //This is not good, we dont have a valid read,
            //lets tell the world (once)...
            QString str("Alarm: Bad sensor");
            myOut() << mqttPublishTopic << str;
            mqtt->pub(mqttPublishTopic, str);

        }
    }

    myErr() << "Problem with sensor getValue";
    return false;
}

QString Regul::timeToSend()
{
    QString res = NULL;
    bool timeToSend = false;


    double diff = input-sentInput;
    if( diff > diffInput || -diff > diffInput )
    {
        myOut() << "Trigger on Sensor value";
        timeToSend = true;
    }

    if(setpoint != sentSetpoint)
    {
        myOut() << "Trigger on Setpoint";
        timeToSend = true;
    }

    diff = output-sentOutput;
    if( diff > diffOutput || -diff > diffOutput )
    {
        myOut() << "Trigger on Output value";
        timeToSend = true;
    }

    //Always send every 10min
    //Time in ms
    if(sendTimer.hasExpired(15*60*1000)) 
    {
        myOut() << "Trigger on sendTimer";
        timeToSend = true;
    }

    if(true == timeToSend)
    {
        sendTimer.restart();

        //value=%d.%02d ; setpoint=%d.%02d ; output=%03d%%"
        res = QString("value=%1 ; setpoint=%2 ; output=%3\%")
            .arg(input,    0, 'f', 2)
            .arg(setpoint, 0, 'f', 2)
            .arg(output,   0, 'f', 0);

        sentInput = input;
        sentSetpoint = setpoint;
        sentOutput = output;
    }
    return res;
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
    /// @todo mqtt->subscribe
}

void Regul::setInput(Sensor *s)
{
    sensor = s;
}

void Regul::setOutput(Output *out)
{
    this->out = out;
}

void Regul::setKp(QString value)
{
    bool ok;
    double tmpvalue = value.toDouble(&ok);
    if(false==ok)
    {
        myErr() << "Bad value" << value;
        return;
    }

    pid->setTuningP(tmpvalue);
}

void Regul::setKi(QString value)
{
    bool ok;
    double tmpvalue = value.toDouble(&ok);
    if(false==ok)
    {
        myErr() << "Bad value" << value;
        return;
    }

    pid->setTuningI(tmpvalue);
}

void Regul::setKd(QString value)
{
    bool ok;
    double tmpvalue = value.toDouble(&ok);
    if(false==ok)
    {
        myErr() << "Bad value" << value;
        return;
    }

    pid->setTuningD(tmpvalue);
}

void Regul::setSetpoint(QString value)
{
    bool ok;
    double tmpvalue = value.toDouble(&ok);
    if(false==ok)
    {
        myErr() << "Bad value" << value;
        return;
    }
    setpoint = tmpvalue;
    pid->setSetpoint(setpoint);
}

void Regul::print()
{
    QString info(name);

    info.append(QString("\n\tsensor=%1 setpoint=%2 output=%3\%")
                    .arg(input,    0, 'f', 2)
                    .arg(setpoint, 0, 'f', 2)
                    .arg(output,   0, 'f', 0));

    info.append(QString("\n\tp=%1 i=%2 d=%3")
        .arg(pid->getKp())
        .arg(pid->getKi())
        .arg(pid->getKd()));

    myOut() << info;

    /*
       if(sensor != NULL)
       {
       sensor->print();
       }
       */
}
