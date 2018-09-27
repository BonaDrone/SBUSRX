/*
   Receiver_example.ino
   Simon D. Levy
   simon.d.levy@gmail.com
   2017-10-26

   Copyright (c) 2016 Bolder Flight Systems

   Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
   and associated documentation files (the "Software"), to deal in the Software without restriction, 
   including without limitation the rights to use, copy, modify, merge, publish, distribute, 
   sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all copies or 
   substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
   BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
   DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

// This example reads an SBUS packet from an
// SBUS receiver (FrSky XM) and reports the
// channel values to the serial monitor

#include "SBUSRX.h"

// a SBUS object, which is on Teensy hardware
// serial port 1
SBUSRX x8r(Serial1);

void setup() 
{
    // begin the SBUS communication
    x8r.begin();

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
    if(x8r.readCal(&channels[0], &failSafe, &lostFrames)){

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

