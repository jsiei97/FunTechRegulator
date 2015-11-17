/**
 * @file PID_v1.cpp
 * @author Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
 * @brief Arduino PID Library - Version 1.0.1
 */

/*
 * Copyright (C) Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
*/

#include "PID_v1.h"

#include "Debug.h"
#include <QElapsedTimer>
#include <QDebug>

/**
 * The parameters specified here are those for for which we can't set up
 * reliable defaults, so we need to have the user set them.
 */
PID::PID(
        double input,
        double output,
        double setpoint,
        double p,
        double i,
        double d,
        int controllerDirection
        )
{
    myOutput   = output;
    myInput    = input;
    mySetpoint = setpoint;
    inAuto     = false;

    //default output limit corresponds to
    //0..100%
    PID::setOutputLimits(0, 100.0);

    //default controller sample Time is 1 seconds
    sampleTime = 1000;

    PID::setControllerDirection(controllerDirection);
    PID::setTunings(p, i, d);

    timer = QElapsedTimer();
    timer.start();
    lastTime = timer.elapsed()-sampleTime;
}


/**
 * compute() This, as they say, is where the magic happens.
 * this function should be called every time "void loop()" executes.
 * the function will decide for itself whether a new
 * pid output needs to be computed.
 *
 * @return true when the output is computed, false when nothing has been done.
 */
bool PID::compute(double input, double *output)
{
    if(!inAuto)
        return false;

    unsigned long now = timer.elapsed();
    unsigned long timeChange = (now - lastTime);

    //myOut() << now << timeChange;

    //if(timeChange >= sampleTime)
    {
        // Compute all the working error variables
        myInput = input;
        double error = mySetpoint - input;
        iTerm += (ki * error);

        if(iTerm > outMax)
        {
            iTerm = outMax;
        }
        else if(iTerm < outMin)
        {
            iTerm = outMin;
        }

        double dInput = (input - lastInput);


        // Compute PID output
        *output = kp * error + iTerm - kd * dInput;

        if(*output > outMax)
        {
            *output = outMax;
        }
        else if(*output < outMin)
        {
            *output = outMin;
        }

        myOutput = *output;

        myOut() << input << mySetpoint << *output;
        // Remember some variables for next time
        lastInput = input;
        lastTime = now;

        return true;
    }

    return false;
}


/**
 * This function allows the controller's dynamic performance to be adjusted.
 * it's called automatically from the constructor, but tunings can also
 * be adjusted on the fly during normal operation
 */
void PID::setTunings(double p, double i, double d)
{
    if (p<0 || i<0 || d<0)
        return;

    dispKp = p;
    dispKi = i;
    dispKd = d;

    double sampleTimeInSec = ((double)sampleTime)/1000;
    kp = p;
    ki = i * sampleTimeInSec;
    kd = d / sampleTimeInSec;

    if(controllerDirection == REVERSE)
    {
        kp = (0 - kp);
        ki = (0 - ki);
        kd = (0 - kd);
    }
}

/**
 * setSampleTime(...) sets the period, in Milliseconds,
 * at which the calculation is performed
 */
void PID::setSampleTime(int newSampleTime)
{
    if (newSampleTime > 0)
    {
        double ratio  = (double)newSampleTime / (double)sampleTime;
        ki *= ratio;
        kd /= ratio;
        sampleTime = (unsigned long)newSampleTime;
    }
}

/**
 * This function will be used far more often than setInputLimits.  while
 * the input to the controller will generally be in the 0-1023 range (which is
 * the default already,)  the output will be a little different.  maybe they'll
 * be doing a time window and will need 0-8000 or something.  or maybe they'll
 * want to clamp it from 0-125.  who knows.  at any rate, that can all be done
 * here.
 */
void PID::setOutputLimits(double min, double max)
{
    if(min >= max)
        return;

    outMin = min;
    outMax = max;

    if(inAuto)
    {
        if(myOutput > outMax)
        {
            myOutput = outMax;
        }
        else if(myOutput < outMin)
        {
            myOutput = outMin;
        }

        if(iTerm > outMax)
        {
            iTerm= outMax;
        }
        else if(iTerm < outMin)
        {
            iTerm= outMin;
        }
    }
}

/**
 * setMode(...) allows the controller Mode to be set to manual (0) or Automatic (non-zero)
 * when the transition from manual to auto occurs, the controller is
 * automatically initialized
 */
void PID::setMode(int Mode)
{
    bool newAuto = (Mode == AUTOMATIC);
    if(newAuto == !inAuto)
    {
        // we just went from manual to auto
        PID::initialize();
    }
    inAuto = newAuto;
}

/**
 * initialize() does all the things that need to happen to ensure a bumpless transfer
 * from manual to automatic mode.
 */
void PID::initialize()
{
    iTerm = myOutput;
    lastInput = myInput;

    if(iTerm > outMax)
    {
        iTerm = outMax;
    }
    else if(iTerm < outMin)
    {
        iTerm = outMin;
    }
}

/**
 * The PID will either be connected to a DIRECT acting process (+Output leads
 * to +Input) or a REVERSE acting process(+Output leads to -Input.)  we need to
 * know which one, because otherwise we may increase the output when we should
 * be decreasing.  This is called from the constructor.
 */
void PID::setControllerDirection(int direction)
{
    if(inAuto && direction != controllerDirection)
    {
        kp = (0 - kp);
        ki = (0 - ki);
        kd = (0 - kd);
    }
    controllerDirection = direction;
}

// Status Funcions:
// Just because you set the kp=-1 doesn't mean it actually happened.  these
// functions query the internal state of the PID.  they're here for display
// purposes.  this are the functions the PID Front-end uses for example
double PID::getKp()
{
    return dispKp;
}

double PID::getKi()
{
    return dispKi;
}

double PID::getKd()
{
    return dispKd;
}

int PID::getMode()
{
    return inAuto ? AUTOMATIC : MANUAL;
}

int PID::getDirection()
{
    return controllerDirection;
}
