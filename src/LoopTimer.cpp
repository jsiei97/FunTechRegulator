/**
 * @file LoopTimer.cpp
 * @author Johan Simonsson
 * @brief A Loop Timer so loop and sleep becomes a fixed time.
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

#include "LoopTimer.h"


LoopTimer::LoopTimer(unsigned long sleepTime)
{
    this->sleepTime = sleepTime;
    timeLast = 0;
}

void LoopTimer::mark(unsigned long time)
{
    timeLast = time;
}

unsigned long LoopTimer::correctedTime(unsigned long time)
{
    unsigned long res = 0;
    if(timeLast < time)
    {
        res = time - timeLast;
        if(res <= sleepTime)
        {
            res = sleepTime-res;
        }
        else
        {
            res = 0;
        }
    }
    else
    {
        //Overflow case
        res = sleepTime;
        if(time < sleepTime)
        {
            res -= time;
        }
    }

    return res;
}
