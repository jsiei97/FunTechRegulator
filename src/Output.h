#ifndef  __OUTPUT_H
#define  __OUTPUT_H

#include "BigPWM.h"
#include "gpio_RaspberryPi.h"
#include "SysfsGPIO.h"

#include <QElapsedTimer>

typedef enum OT
{
    OUTPUT_NONE = 0, ///< No configured output
    OUTPUT_PWM_GPIO  ///< BigPWM on a gpio pin
} OutputType;

class Output
{
    private:
        OutputType type;

        BigPWM *pwm;
        GPIO_Pin gpiopin;
        QElapsedTimer timer;

        QString name;
        QString in;

    public:
        Output();

        void setName(QString name);
        void setInput(QString out);

        bool putValue(double value);
        void setPwmGpio(QString gpionr);

        void print();
};

#endif  // __OUTPUT_H
