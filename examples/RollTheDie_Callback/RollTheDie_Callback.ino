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
 *         File: RollTheDie_Callback.ino
 *      Created: 2018-10-22
 *       Author: Geoffrey Ottoy
 *
 *  Description: Callback-based operation of DIE SHIELD
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
 
#include "die-shield.h"

#define ROLLING_TRESHOLD 500 // ms

#define BUTTON_S1 2 // D2 (INT0)

bool buttonPressed = false;
bool newRoll = false;
bool dieShaking = false;
bool dieRolling = false;
unsigned long prevShakeTime = 0;  
unsigned int shakeCounter = 0;

/* ISR for Button S1 */
void s1Pressed(void){
  if(!newRoll){ // ignore button press when roll cycle has started
    buttonPressed = true;
  }
}

/* Callback for the Die shake event */
ShakeCallback_t iAmShaking(void){
  if(!dieRolling){ // ignore shake when roll animation has started
    dieShaking = true;
  }
}

void setup(){
  Serial.begin(115200);
  // Initialize Button S1
  pinMode(BUTTON_S1, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_S1), s1Pressed, FALLING);
  
  // Initialize Die Shield for callback-based operation
  Die.attachShakeCallback(&iAmShaking);
  Die.begin();
}

void loop(){
  // take time stamp
  unsigned long shakeTime = millis();

  // shake event has been called
  if(dieShaking){
    if(shakeCounter==0){ // first shake (of the roll cylce)
      // show a new value
      Die.roll();
    }
    else{ // another shake
      // start roll cycle
      newRoll = true; 
      // show a new value
      Die.roll();
    }
    // count the number of shakes
    shakeCounter++;

    // store the time of this shake
    prevShakeTime = shakeTime;
    
    // store the time of this shake
    dieShaking = false;
  }

  // roll cycle has started
  if(newRoll){
    // detect if shaking has stopped (die is released)
    if(shakeTime-prevShakeTime > ROLLING_TRESHOLD){
       // to ignore new shakes while die is rolling
      dieRolling = true;
       // start roll animation (the more shakes have been counted, the longer it will roll)
      Die.roll(shakeCounter);
      
      // end roll cycle
      shakeCounter = 0; 
      newRoll = false;
      dieRolling = false;
    }
  }

  // button has been pressed (start roll animation without shaking)
  if(buttonPressed){
     // start roll animation
    Die.roll(10);
    // button press has been handled
    buttonPressed = false;
  }
  
}
