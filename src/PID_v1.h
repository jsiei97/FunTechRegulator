/**
 * @file PID_v1.h
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


#ifndef PID_v1_h
#define PID_v1_h

#include <QElapsedTimer>

//Constants used in some of the functions below
#define AUTOMATIC	1
#define MANUAL	0
#define DIRECT  0
#define REVERSE  1

class PID
{
    public:
        /**
         * Links the PID to the Input, Output, and Setpoint.
         * Initial tuning parameters are also set here
         */
        PID(double, double, double, double, double, double, int);

        /**
         * Sets PID to either Manual (0) or Auto (non-0)
         */
        void setMode(int mode);

        /**
         * Performs the PID calculation.
         *
         * It should be called every time loop() cycles.
         * ON/OFF and calculation frequency can be set using SetMode
         * SetSampleTime respectively
         */
        bool compute(double input, double *output);

        /**
         * Clamps the output to a specific range.
         *
         * clamps the output to a specific range, 0-255 by default,
         * but it's likely the user will want to change this depending on
         * the application
         */
        void setOutputLimits(double, double);


        /**
         * While most users will set the tunings once in the
         * constructor, this function gives the user the option
         * of changing tunings during runtime for Adaptive control
         */
        void setTunings(double, double, double);

        /**
         * Sets the Direction, or "Action" of the controller.
         * DIRECT means the output will increase when error is positive.
         * REVERSE means the opposite.
         * It's very unlikely that this will be needed once it is set in the constructor.
         */
        void setControllerDirection(int);

        /**
         * Sets the frequency, in Milliseconds, with which
         * the PID calculation is performed.  default is 100
         */
        void setSampleTime(int);



        //Display functions ****************************************************************
        // These functions query the pid for interal values.
        // they were created mainly for the pid front-end,
        // where it's important to know what is actually
        // inside the PID.
        double getKp();
        double getKi();
        double getKd();
        int getMode();
        int getDirection();

    private:
        QElapsedTimer timer;

        void initialize();

        // we'll hold on to the tuning parameters in user-entered
        // format for display purposes
        double dispKp;
        double dispKi;
        double dispKd;

        double kp;  ///< (P)roportional Tuning Parameter
        double ki;  ///< (I)ntegral Tuning Parameter
        double kd;  ///< (D)erivative Tuning Parameter

        int controllerDirection;

        // Pointers to the Input, Output, and Setpoint variables
        // This creates a hard link between the variables and the
        // PID, freeing the user from having to constantly tell us
        // what these values are.  with pointers we'll just know.
        double myInput;
        double myOutput;
        double mySetpoint;

        unsigned long lastTime;
        double iTerm, lastInput;

        unsigned long sampleTime;
        double outMin, outMax;
        bool inAuto;
};
#endif
