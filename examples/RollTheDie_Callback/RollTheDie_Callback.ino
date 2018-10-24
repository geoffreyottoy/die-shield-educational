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

#define ROLLING_TRESHOLD 500

#define BUTTON_S1 2

bool buttonPressed = false;
bool dieShaking = false;
bool dieRolling = false;
unsigned long prevShakeTime = 0;  
unsigned int shakeCounter = 0;

void s1Pressed(void){
  buttonPressed = true;
}

static void iAmShaking(void){
  unsigned long shakeTime = millis();
  shakeCounter++;
  
  if(shakeTime-prevShakeTime > ROLLING_TRESHOLD){
    dieRolling = true;
  }
  else{
    dieShaking = true;
  }
  
  prevShakeTime = shakeTime;
}

void setup(){
  pinMode(BUTTON_S1, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_S1), s1Pressed, FALLING);
  
  // put your setup code here, to run once:
  Die.attachShakeCallback(&iAmShaking);
  Die.begin();
}

void loop(){
  if(dieShaking){
    dieShaking = false;
	Die.roll();
  }
  
  if(dieRolling){
    dieRolling = false;
	shakeCounter = 0;
	Die.roll(shakeCounter);
  }
  
  if(buttonPressed){
    buttonPressed = false;
    Die.roll(10);
  }
  
}
