#include <QSettings>
#include <QDebug>
#include <QStringList>
#include <QRegExp>
#include <QElapsedTimer>

#include "Regul.h"
#include "Sensor.h"
#include "Debug.h"

#include "Output.h"
#include "LoopTimer.h"
#include "MosqConnect.h"
#include <mosquitto.h>
#define DATA_LENGTH 160
#define MAX_SIZE 100
MosqConnect *mqtt;

int main()
{
    int rc;
    mosqpp::lib_init();
    mqtt = new MosqConnect(
            "FunTechRegulator",
            "mosqhub",
            1883
            );

    /// @todo setting file as arg
    QSettings settings("test.ini", QSettings::IniFormat);

    //Read config file and create, sensor, pid and output
    QHash<QString, Regul> regulators;
    QHash<QString, Sensor> sensors;
    QHash<QString, Output> outputs;

    settings.beginGroup("regul");
    {
        const QStringList childKeys = settings.childKeys();
        foreach (const QString &childKey, childKeys)
        {
            QString key   = childKey;
            QString value = settings.value(childKey).toString();
            //myOut() << key << value;
            //create and store a regul/pid in a hash

            QString name, action;
            QRegExp rx("(.*)\\.(.*)");
            if (rx.indexIn(key) != -1) {
                name  = rx.cap(1);
                action = rx.cap(2);
            }
            myOut() << name << action << value;

            if(action.contains("name", Qt::CaseInsensitive))
            {
                regulators[name].setName(value);
            }
            else if(action.contains("publish", Qt::CaseInsensitive))
            {
                regulators[name].setMQTTpublish(value, mqtt);
            }
            else if(action.contains("subscribe", Qt::CaseInsensitive))
            {
                regulators[name].setMQTTsubscribe(value, mqtt);
            }
            //The PID variables
            else if(action.contains("kp", Qt::CaseInsensitive))
            {
                regulators[name].setKp(value);
            }
            else if(action.contains("ki", Qt::CaseInsensitive))
            {
                regulators[name].setKi(value);
            }
            else if(action.contains("kd", Qt::CaseInsensitive))
            {
                regulators[name].setKd(value);
            }
            else if(action.contains("setpoint", Qt::CaseInsensitive))
            {
                regulators[name].setSetpoint(value);
            }

        }
        settings.endGroup();
        myOut() << "";
    }

    settings.beginGroup("sensor");
    {
        const QStringList childKeys = settings.childKeys();
        foreach (const QString &childKey, childKeys)
        {
            QString key   = childKey;
            QString value = settings.value(childKey).toString();
            //myOut() << key << value;
            //create sensor

            QString name, action;
            QRegExp rx("(.*)\\.(.*)");
            if (rx.indexIn(key) != -1) {
                name  = rx.cap(1);
                action = rx.cap(2);
            }
            myOut() << name << action << value;

            if(action.contains("name", Qt::CaseInsensitive))
            {
                sensors[name].setName(value);
            }
            else if(action.contains("output", Qt::CaseInsensitive))
            {
                //Make sure that this output goes to regulators input.
                sensors[name].setOut(value);
                regulators[value].setInput(&sensors[name]);
            }
            else if(action.contains("onewire", Qt::CaseInsensitive))
            {
                sensors[name].setOneWire(value);
            }
        }
        settings.endGroup();
        myOut() << "";
    }

    settings.beginGroup("output");
    {
        const QStringList childKeys = settings.childKeys();
        foreach (const QString &childKey, childKeys)
        {
            QString key   = childKey;
            QString value = settings.value(childKey).toString();
            //myOut() << key << value;
            //create sensor

            QString name, action;
            QRegExp rx("(.*)\\.(.*)");
            if (rx.indexIn(key) != -1) {
                name  = rx.cap(1);
                action = rx.cap(2);
            }
            myOut() << name << action << value;

            if(action.contains("name", Qt::CaseInsensitive))
            {
                outputs[name].setName(value);
            }
            else if(action.contains("input", Qt::CaseInsensitive))
            {
                //Make sure that this input comes from a regulators output.
                outputs[name].setInput(value);
                regulators[value].setOutput(&outputs[name]);
            }
            else if(action.contains("pwm_gpio", Qt::CaseInsensitive))
            {
                outputs[name].setPwmGpio(value);
            }
            // Add PWM window size
        }
        settings.endGroup();
        myOut() << "";
    }

    //Then check and init all the regulators.
    {
        QHashIterator<QString, Regul> i(regulators);
        while (i.hasNext()) {
            i.next();
            //myOut() << i.key();
            regulators[i.key()].print();
            int err = regulators[i.key()].start();
            if(0 != err)
            {
                myErr() << "regul start failed:" << i.key() << "error" << err;
                return err;
            }
        }
    }

    unsigned long sleepTime = 1000; //1s
    LoopTimer loopTimer(sleepTime);

    unsigned long sleep = 0; 
    QElapsedTimer timer;
    timer.start();


    myOut() << "Lets start...";
    while(true)
    {
        loopTimer.mark(timer.elapsed());
        //myOut() << "Time:" << timer.elapsed() << "ms Loop:" << sleep << "ms";

        QHashIterator<QString, Regul> i(regulators);
        while (i.hasNext()) {
            i.next();
            //myOut() << i.key();
            if(!(regulators[i.key()].compute()))
            {
                myErr() << i.key() << "failed to compute";
            }
            //regulators[i.key()].print();
        }


        
        //This check is a 1800ms operation...?
        //switch to 
        //http://mosquitto.org/api/files/mosquitto-h.html#mosquitto_loop_forever
        rc = mqtt->loop();
        //myOut() << "rc" << rc;
        if(rc)
        {
            myOut() << "reconnect...";
            mqtt->reconnect();
        }

        sleep = loopTimer.correctedTime(timer.elapsed());
        if(sleep!=0)
        {
            usleep(sleep*1000);
        }
    }

    mosqpp::lib_cleanup();
    return 0;
}
