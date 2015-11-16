/**
 * @file BigPWM.cpp
 * @author Johan Simonsson
 * @brief A big window pwm class
 */

/*
 * Copyright (C) 2014 Johan Simonsson
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

#include "BigPWM.h"

BigPWM::BigPWM()
{
    windowSize = 1*60*1000; // 1 min default
    reset();
}

void BigPWM::reset()
{
    state = PWM_T0;
    t0 = 0;
    t1 = 0;
    t2 = 0;
}

void BigPWM::setWindow(unsigned long windowSize)
{
    this->windowSize = windowSize;
}

/**
 * calculate the output from the BigPWM
 *
 * @param time the current time in milliseconds
 * @param out out signal from 0..100, where 100 is 100%
 * @return true if out is high, false if low.
 */
bool BigPWM::calc(unsigned long time, unsigned long out)
{
    if(state!=PWM_T0 && time<=t0)
    {
        //This is bad since time should not be smaller than when we started!
        //reset and go back to a known state!
        reset();
    }

    switch ( state )
    {
        default:
            //This should not happen, but if so go back to known state.
            reset();
        case PWM_T0:
            {
                t0=time;
                t2=time+windowSize;
                if(0==out)
                {
                    state=PWM_MIN;
                    return false;
                }
                else if(out>=100)
                {
                    state=PWM_MAX;
                    return true;
                }
                else
                {
                    state=PWM_T1;
                    //With 20% on, start with 80% off
                    //With 80% on, start with 20% off
                    t1=time+((windowSize*(100-out))/100);
                    return false;
                }
            }
            break;

        case PWM_T1:
            {
                if(time>=t1)
                {
                    state=PWM_MAX;
                    return true;
                }
                return false;
            }
            break;

        case PWM_MAX:
            {
                if(time>=(t2-1))
                {
                    state=PWM_T0;
                }
                return true;
            }
            break;

        case PWM_MIN:
            {
                if(time>=(t2-1))
                {
                    state=PWM_T0;
                }
                return false;
            }
            break;
    }

    return false;
}
