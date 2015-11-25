
#include <QDebug>
#include <QString>
#include <QElapsedTimer>

#include "Output.h"
#include "BigPWM.h"
#include "gpio_RaspberryPi.h"
#include "SysfsGPIO.h"

#include "Debug.h"

Output::Output()
{
    type = OUTPUT_NONE;

    pwm = NULL;
    timer.start();
}

void Output::setName(QString name)
{
    this->name = name;
}

void Output::setInput(QString in)
{
    this->in = in;
}

void Output::setPwmGpio(QString gpionr)
{
    gpiopin = SysfsGPIO::checkGPIO(gpionr);
    if(GPIO_NONE == gpiopin)
    {
        myErr() << "bad gpio num..." << gpionr;
        return;    
    }

    type = OUTPUT_PWM_GPIO;
    pwm = new BigPWM();
    pwm->setWindow(20*1000); // 20s

    if(!SysfsGPIO::configureGPIO(gpiopin, GPIO_DIRECTION_OUTPUT))
    {
        myErr() << "configure failed...";
        return;
    }
    SysfsGPIO::writeGPIO(gpiopin, GPIO_LOW);
}


bool Output::putValue(double value)
{
    //qDebug() << "putValue"  << value;
    switch ( type )
    {
        case OUTPUT_PWM_GPIO :
            {
                //Make sure that output is 0..100 before this point...
                bool outstate = pwm->calc(timer.elapsed(), (unsigned long)value);
                SysfsGPIO::writeGPIO(gpiopin, outstate);
                //myOut() << "PWM:" << outstate << "Out:" << value << "%";
            }
            break;
        case OUTPUT_NONE :
        default :
            // ???
            break;
    }

    return false;
}

void Output::print()
{
    myOut() << this->name;
}
