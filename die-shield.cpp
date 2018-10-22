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
 *         File: die-shield.cpp
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
 
#include <Arduino.h>
#include "die-shield.h"

// This will store 1 instance.
static DieShield * instance = 0;

volatile int sampleCtr;
volatile int rollingCounter;

static void (*callback)();

uint8_t ledLUT[10][7]={
    {0,0,0,0,0,0,0}, // NONE
    {0,0,0,0,0,0,1}, // 1
    {1,0,0,1,0,0,0}, // 2
    {1,0,0,1,0,0,1}, // 3
    {1,0,1,1,0,1,0}, // 4
    {1,0,1,1,0,1,1}, // 5
    {1,1,1,1,1,1,0}, // 6
    {1,1,1,1,1,1,1}, // ALL
	// rotated view
    {0,0,1,0,0,1,0}, // 2
    {0,0,1,0,0,1,1}, // 3
};

ISR(TIMER1_COMPA_vect){
	if(instance->rolling){
		rollingCounter++;
	}
	instance->detectShake();
}

DieShield::DieShield(){
	if(instance == NULL){
        instance = this;
    }
	this->rolling = false;
	
	this->_clkPin = DEFAULT_CLK_PIN;
	this->_led0Pin = DEFAULT_LED0_PIN;
	this->_xAxisPin = DEFAULT_X_AXIS_PIN;
	this->_yAxisPin = DEFAULT_Y_AXIS_PIN;
	this->_zAxisPin = DEFAULT_Z_AXIS_PIN;
	
	this->x.init(BUFFER_SIZE);
	this->y.init(BUFFER_SIZE);
	this->z.init(BUFFER_SIZE);
}

void DieShield::begin(){
	sampleCtr=0;
	randomSeed(analogRead(A5));
    this->pinsInit();
	this->timerOneInit();
}

void DieShield::roll(){
	uint8_t value = random(1,7);
	uint8_t offset = 0;
	if((value==2)||(value==3)){
		offset = ROTATED_OFFSET * random(0, 2);
	}
	this->show(value);
}

void DieShield::roll(int duration){
	this->rolling = true;
	int scaledDuration = duration * 100;
	int increase = 20;
	while(rollingCounter < scaledDuration){
		if(rollingCounter == increase){
			if(increase < 300){
				increase += 20;
			}else{
				if(increase < 600){
					increase += 50;
				}
				else{
					increase += 100;
				}
			}
			this->roll();
		}
	}
	rollingCounter = 0;
	this->rolling = false;
	// Done rolling show final value
	uint8_t value = random(1,7);
	show(NONE);
	delay(1000);
	show(value);
	delay(250);
	show(NONE);
	delay(250);
	show(value);
}

void DieShield::show(uint8_t value){
	uint8_t * leds = ledLUT[value];
	this->updateLeds(leds);
}

void DieShield::detectShake(){
	sampleCtr++;
	bool shakeDetected = false;
	float x_val = (float)5*analogRead(this->_xAxisPin)/1024;
	float y_val = (float)5*analogRead(this->_yAxisPin)/1024;
	float z_val = (float)5*analogRead(this->_zAxisPin)/1024;
	
	this->x.put(x_val);
	this->y.put(y_val);
	this->z.put(z_val);
	
	// buffer is renewed
	if(sampleCtr==BUFFER_SIZE){
		sampleCtr=0;
		float xAvg = this->x.getAverage();
		float yAvg = this->y.getAverage();
		float zAvg = this->z.getAverage();
		// detect shake
		if(abs(xAvg - this->prevX) > SHAKE_SENSITIVITY){
			// shake detected
			shakeDetected = true;
		}
		if(abs(yAvg - this->prevY) > SHAKE_SENSITIVITY){
			// shake detected
			shakeDetected = true;
		}
		if(abs(zAvg - this->prevZ) > SHAKE_SENSITIVITY){
			// shake detected
			shakeDetected = true;
		}
		// update previous value
		this->prevX = xAvg;
		this->prevY = yAvg;
		this->prevZ = zAvg;
		// execute callback
		if(shakeDetected){ // callback
			callback();
		}
	}
}

void DieShield::attachShakeCallback(void (* cb)()){
	callback = cb;
}

void DieShield::pinsInit(){
	pinMode(this->_clkPin, OUTPUT);
	pinMode(this->_led0Pin, OUTPUT);
	digitalWrite(this->_clkPin, LOW);
	pinMode(this->_xAxisPin, INPUT);
    pinMode(this->_yAxisPin, INPUT);
    pinMode(this->_zAxisPin, INPUT);
}

void DieShield::timerOneInit(){
	#if defined(__AVR__)
	cli();
	//set timer1 interrupt at 200Hz
	TCCR1A = 0;// set entire TCCR1A register to 0
	TCCR1B = 0;// same for TCCR1B
	TCNT1  = 0;//initialize counter value to 0
	// set compare match register for 1hz increments
	OCR1A = 9999;// = ((16*10^6) / (200*8)) - 1 (must be <65536)
	// turn on CTC mode
	TCCR1B |= (1 << WGM12);
	// set CS11 bit for /8 prescaler
	TCCR1B |= (1 << CS11);  
	// enable timer compare interrupt
	TIMSK1 |= (1 << OCIE1A);
	
	sei();
	#else
		#error "Only AVR controllers are supported (for now)."
	#endif
	Serial.println("Timer 1 initialized.");
}

void DieShield::updateLeds(uint8_t * leds){
	uint8_t i;
	for(i=0; i<6; i++){
		digitalWrite(this->_led0Pin, leds[i]);
		digitalWrite(this->_clkPin, HIGH);
		digitalWrite(this->_clkPin, LOW);
	}
	digitalWrite(this->_led0Pin, leds[6]);
}