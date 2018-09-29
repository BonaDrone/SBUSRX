/*
   SBUSRX.cpp
   Brian R Taylor, Simon D. Levy

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

#include "SBUSRX.h"

/* starts the serial communication */
void SBUSRX::begin()
{
    // initialize parsing state
    _fpos = 0;
}

/* read the SBUS data and calibrate it to +/- 1 */
bool SBUSRX::readCal(float* calChannels, uint8_t* failsafe, uint16_t* lostFrames)
{
    uint16_t channels[16];

    // read the SBUS data
    if(read(&channels[0],failsafe,lostFrames)){

        // linear calibration
        for(uint8_t i = 0; i < 16; i++){
            calChannels[i] = channels[i] * _sbusScale + _sbusBias;
        }

        // return true on receiving a full packet
        return true;
    }
    else{

        // return false if a full packet is not received
        return false;
    }
}

/* read the SBUS data */
bool SBUSRX::read(uint16_t* channels, uint8_t* failsafe, uint16_t* lostFrames)
{
    // parse the SBUS packet
    if (parse()) {

        // 16 channels of 11 bit data
        channels[0]  = (int16_t) ((_payload[0]    |_payload[1]<<8)                          & 0x07FF);
        channels[1]  = (int16_t) ((_payload[1]>>3 |_payload[2]<<5)                          & 0x07FF);
        channels[2]  = (int16_t) ((_payload[2]>>6 |_payload[3]<<2 |_payload[4]<<10)  		& 0x07FF);
        channels[3]  = (int16_t) ((_payload[4]>>1 |_payload[5]<<7)                          & 0x07FF);
        channels[4]  = (int16_t) ((_payload[5]>>4 |_payload[6]<<4)                          & 0x07FF);
        channels[5]  = (int16_t) ((_payload[6]>>7 |_payload[7]<<1 |_payload[8]<<9)   		& 0x07FF);
        channels[6]  = (int16_t) ((_payload[8]>>2 |_payload[9]<<6)                          & 0x07FF);
        channels[7]  = (int16_t) ((_payload[9]>>5 |_payload[10]<<3)                         & 0x07FF);
        channels[8]  = (int16_t) ((_payload[11]   |_payload[12]<<8)                         & 0x07FF);
        channels[9]  = (int16_t) ((_payload[12]>>3|_payload[13]<<5)                         & 0x07FF);
        channels[10] = (int16_t) ((_payload[13]>>6|_payload[14]<<2|_payload[15]<<10) 		& 0x07FF);
        channels[11] = (int16_t) ((_payload[15]>>1|_payload[16]<<7)                         & 0x07FF);
        channels[12] = (int16_t) ((_payload[16]>>4|_payload[17]<<4)                         & 0x07FF);
        channels[13] = (int16_t) ((_payload[17]>>7|_payload[18]<<1|_payload[19]<<9)  		& 0x07FF);
        channels[14] = (int16_t) ((_payload[19]>>2|_payload[20]<<6)                         & 0x07FF);
        channels[15] = (int16_t) ((_payload[20]>>5|_payload[21]<<3)                         & 0x07FF);

        // count lost frames
        if (_payload[22] & _sbusLostFrame) {
            *lostFrames = *lostFrames + 1;
        }

        // failsafe state
        if (_payload[22] & _sbusFailSafe) {
            *failsafe = 1;
        } 
        else{
            *failsafe = 0;
        }

        // return true on receiving a full packet
        return true;
    }
    else{

        // return false if a full packet is not received
        return false;
    }
}

/* parse the SBUS data */
bool SBUSRX::parse()
{
    // A workaround to emulate Teensy's elapsedTime support
    static uint32_t startTime;
    static uint32_t sbusTime;
    uint32_t currTime = micros();
    sbusTime = currTime - startTime;
    startTime = currTime;

    if(sbusTime > SBUS_TIMEOUT){_fpos = 0;}

    // see if serial data is available
    while(sbusSerialAvailable() > 0){
        sbusTime = 0;
        static uint8_t c;
        static uint8_t b;
        c = sbusSerialRead();

        // find the header
        if(_fpos == 0){
            if((c == _sbusHeader)&&((b == _sbusFooter)||((b & 0x0F) == _sbus2Footer))){
                _fpos++;
            }
            else{
                _fpos = 0;
            }
        }
        else{

            // strip off the data
            if((_fpos-1) < _payloadSize){
                _payload[_fpos-1] = c;
                _fpos++;
            }

            // check the end byte
            if((_fpos-1) == _payloadSize){
                if((c == _sbusFooter)||((c & 0x0F) == _sbus2Footer)) {
                    _fpos = 0;
                    return true;
                }
                else{
                    _fpos = 0;
                    return false;
                }
            }
        }
        b = c;
    }
    // return false if a partial packet
    return false;
}
