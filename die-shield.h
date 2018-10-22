/*  ____  ____      _    __  __  ____ ___
 * |  _ \|  _ \    / \  |  \/  |/ ___/ _ \
 * | | | | |_) |  / _ \ | |\/| | |  | | | |
 * | |_| |  _ <  / ___ \| |  | | |__| |_| |
 * |____/|_| \_\/_/   \_\_|  |_|\____\___/
 *                           research group
 *                             dramco.be/
 *
 *  KU Leuven - Technology Campus Gent,
 *  Gebroeders De Smetstraat 1,
 *  B-9000 Gent, Belgium
 *
 *         File: die-shield.h
 *      Created: 2018-10-21
 *       Author: Geoffrey Ottoy
 *      Version: 1.0
 *
 *  Description: text
 *      some more text
 *
 * MIT License
 * 
 * Copyright (c) 2018 DRAMCO - KU Leuven
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
 
#ifndef __DIE_SHIELD_H__
#define __DIE_SHIELD_H__

#include <Arduino.h>
#include "circ-buffer.h"

#define SHAKE_SENSITIVITY	0.25
#define BUFFER_SIZE			20

#define DEFAULT_CLK_PIN		7
#define DEFAULT_LED0_PIN	4

#define DEFAULT_X_AXIS_PIN  A0
#define DEFAULT_Y_AXIS_PIN	A1
#define DEFAULT_Z_AXIS_PIN	A2

#define NONE				0
#define ALL					7
#define ROTATED_OFFSET		6

class DieShield{
	public:
		 // Static access method.
		DieShield();
	
		void begin();
	
		void roll();
		void roll(int duration);
		void show(uint8_t value);
	
		void detectShake();
	
		void attachShakeCallback(void (* cb)());
		bool rolling;
	
	private:
		void pinsInit();
		void timerOneInit();
		void updateLeds(uint8_t * leds);
		
		CircBuffer x;
		CircBuffer y;
		CircBuffer z;
		
		float prevX;
		float prevY;
		float prevZ;
		
		// pin numbers
		uint8_t _clkPin;
		uint8_t _led0Pin;
		uint8_t _xAxisPin;
		uint8_t _yAxisPin;
		uint8_t _zAxisPin;
		
};

#endif /*__DIE_SHIELD_H__*/
