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
 *
 *  Description: Implementation of the DieShield class. This class offers an API to
 *               the DRAMCO DIE SHIELD.
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

DieShield Die;              // preinstatiate

volatile int sampleCtr;
//volatile int rollingCounter;

static bool dsInitialized = false;

static ShakeCallback_t scb = NULL;

uint8_t ledLUT[8][7]={
    {0,0,0,0,0,0,0}, // NONE
    {0,0,0,0,0,0,1}, // 1
    {1,0,0,1,0,0,0}, // 2
    {1,0,0,1,0,0,1}, // 3
    {1,0,1,1,0,1,0}, // 4
    {1,0,1,1,0,1,1}, // 5
    {1,1,1,1,1,1,0}, // 6
    {1,1,1,1,1,1,1}, // ALL
};

ISR(TIMER1_COMPA_vect){
	// for Rolling animation
	/*if(Die.rolling){
		rollingCounter++;
	}*/
	
	// Sample ADXL335
	sampleCtr++;
	Die.readADXL335();
	
	// Detect Shake
	if(sampleCtr==BUFFER_SIZE){
		sampleCtr=0;
		if(Die.detectShake() == DS_SHAKING){
			scb();
		}
	}
	
}

// Constructor (set pins)
DieShield::DieShield(){	
	// Set pin numbers
	this->clkPin = DEFAULT_CLK_PIN;
	this->led0Pin = DEFAULT_LED0_PIN;
	this->xAxisPin = DEFAULT_X_AXIS_PIN;
	this->yAxisPin = DEFAULT_Y_AXIS_PIN;
	this->zAxisPin = DEFAULT_Z_AXIS_PIN;
}

DieStatus_t DieShield::attachShakeCallback(ShakeCallback_t cb){
	if(scb != NULL){
		return DS_NOT_NULL;
	}
	scb = cb;
	
	return DS_SUCCESS;
}

DieStatus_t DieShield::begin(){
	// Avoid more than one instance
	if(dsInitialized){
		return DS_INIT_ERROR;
	}
	dsInitialized = true;
	
	// Initialize attributes
	sampleCtr=0;
	this->rolling = false;
	// Initialize random generator
	randomSeed(analogRead(A5));
	// Initialize measurement buffers
	if(this->initBuffers(BUFFER_SIZE) != CB_SUCCESS){
		return DS_INIT_ERROR;
	}
	// Initialize pins
    this->pinsInit();
	// Initialize timer 1
	this->timerOneInit();
	
	return DS_SUCCESS;
}

/* Generate random value and show
 */
void DieShield::roll(){
	DieValue_t value = (DieValue_t) random(1,7);
	this->show(value);
}

/* Roll animation
 */
void DieShield::roll(int nrShakes){
	int i;
	int rem = (nrShakes-3)*4;
	if(nrShakes>3){
		for(i=0;i<rem;i++){
			this->roll();
			delay(100);
		}
	}
	for(i=0;i<4;i++){
		this->roll();
		delay(200);
	}
	this->roll();
	delay(300);
	this->roll();
	delay(300);
	this->roll();
	delay(600);
	
	// Done rolling show final value
	DieValue_t value = (DieValue_t) random(1,7);
	this->show(value);
	delay(1000);
	this->show(NONE);
	delay(400);
	this->show(value);
	delay(200);
	this->show(NONE);
	delay(200);
	this->show(value);
}

void DieShield::show(DieValue_t value){
	uint8_t * leds = ledLUT[value];
	uint8_t i;
	for(i=0; i<6; i++){
		digitalWrite(this->led0Pin, leds[i]);
		digitalWrite(this->clkPin, HIGH);
		digitalWrite(this->clkPin, LOW);
	}
	digitalWrite(this->led0Pin, leds[6]);
}

DieStatus_t DieShield::detectShake(void){
	bool shakeDetected = false;

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
		return DS_SHAKING;
	}
	
	return DS_NOT_SHAKING;
}

/*****************************************************************************
 * Private methods                                                           *
 *****************************************************************************/

void DieShield::pinsInit(){
	// ADXL335 (analog) inputs
	pinMode(this->xAxisPin, INPUT);
    pinMode(this->yAxisPin, INPUT);
    pinMode(this->zAxisPin, INPUT);
	
	// LED control
	pinMode(this->clkPin, OUTPUT);
	pinMode(this->led0Pin, OUTPUT);
	
	// Init clock line (LOW)
	digitalWrite(this->clkPin, LOW);
}

void DieShield::timerOneInit(){
#if defined(__AVR__)
	cli();
	//set timer1 interrupt at 200 Hz
	TCCR1A = 0; // set entire TCCR1A register to 0
	TCCR1B = 0; // same for TCCR1B
	TCNT1  = 0; //initialize counter value to 0
	// set compare match register for 200 Hz increments
	OCR1A = 9999; // = ((16*10^6) / (200*8)) - 1 (must be <65536)
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
}

CircBufferStatus_t DieShield::initBuffers(uint8_t size){
	CircBufferStatus_t status = this->x.init(size);
	if(status != CB_SUCCESS){
		return status;
	}
	status = this->y.init(size);
	if(status != CB_SUCCESS){
		return status;
	}
	status = this->z.init(size);
	if(status != CB_SUCCESS){
		return status;
	}
	return CB_SUCCESS;
}

void DieShield::readADXL335(void){
	// Read ADXL335 voltages
	float x_val = (float)5*analogRead(this->xAxisPin)/1024;
	float y_val = (float)5*analogRead(this->yAxisPin)/1024;
	float z_val = (float)5*analogRead(this->zAxisPin)/1024;
	
	// Store measurements into buffer
	this->x.put(x_val);
	this->y.put(y_val);
	this->z.put(z_val);
}