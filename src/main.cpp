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

int main()
{
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
                /// @todo create...
                //outputs[name].setPwmGpio(value);
            }
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
    QElapsedTimer timer;
    timer.start();


    myOut() << "Print results:";
    for( int i=0 ; i<10 ; i++ )
    {
        loopTimer.mark(timer.elapsed());
        myOut() << "Loop:" << timer.elapsed() << "ms";

        QHashIterator<QString, Regul> i(regulators);
        while (i.hasNext()) {
            i.next();
            //myOut() << i.key();
            if(!(regulators[i.key()].compute()))
            {
                myErr() << i.key() << "failed to compute";
            }
            regulators[i.key()].print();
        }
        usleep(loopTimer.correctedTime(timer.elapsed())*1000);
    }
    return 0;
}
