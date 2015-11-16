
#include <QDebug>
#include <QString>

#include "Output.h"
#include "Debug.h"

Output::Output()
{
    type = OUTPUT_NONE;
}

void Output::setName(QString name)
{
    this->name = name;
}

void Output::setInput(QString in)
{
    this->in = in;
}


bool Output::putValue(double value)
{
    qDebug() << "putValue"  << value;
    switch ( type )
    {
        case OUTPUT_PWM_GPIO :
            /// @todo update the pwm...
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
