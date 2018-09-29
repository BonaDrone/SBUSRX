/*
   STM32L4.ino SBUSRX example for STM32L4 boards

   Copyright (C) Simon D. Levy 2017

   This file is part of SBUSRX.

   SBUSRX is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   SBUSRX is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with SBUSRX.  If not, see <http://www.gnu.org/licenses/>.
 */

// This example reads an SBUS packet from an
// SBUS receiver (FrSky XM) and reports the
// channel values to the serial monitor

#include "SBUSRX.h"

// Required for SBUSRX

uint8_t sbusSerialAvailable(void)
{
    return Serial1.available();
}

uint8_t sbusSerialRead(void)
{
    return Serial1.read();
}

SBUSRX rx;

void setup() 
{
    // begin the serial port for SBUS
    Serial1.begin(100000, SERIAL_SBUS);

    // begin the SBUS communication
    rx.begin();

    // begin serial-monitor communication
    Serial.begin(115200);
}

static void showaxis(const char * label, float axval)
{
    char tmp[100];
    sprintf(tmp, "%s: %+2.2f  ", label, axval);
    Serial.print(tmp);
}

void loop() 
{
    uint8_t failSafe;
    uint16_t lostFrames = 0;
    float channels[16];

    // look for a good SBUS packet from the receiver
    if(rx.readCal(&channels[0], &failSafe, &lostFrames)){

        // First five channels (Throttle, Aieleron, Elevator, Rudder, Auxiliary) are enough to see whether it's working
        showaxis("Thr",  channels[0]);
        showaxis("Ael",  channels[1]);
        showaxis("Ele",  channels[2]);
        showaxis("Rud",  channels[3]);
        showaxis("Aux1", channels[4]);
        showaxis("Aux2", channels[5]);
        Serial.print("    Failsafe: ");
        Serial.print(failSafe);
        Serial.print("    Lost frames: ");
        Serial.println(lostFrames);
     }
}

