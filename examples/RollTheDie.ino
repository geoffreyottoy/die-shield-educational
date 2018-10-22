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
 *         File: RollTheDie.ino
 *      Created: 2018-10-22
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
 
#include "die-shield.h"

#define POLL_INTERVAL 250

#define S1 2

DieShield die;

bool buttonPressed = false;
bool dieRolled = false;
unsigned long previousMillis = 0;  
unsigned int noRollCounter = 0;
int rollDuration = 0;

typedef enum appStates{UPDATE_TIME, CHECK_DIE_ROLL, ANIMATE_ROLL} AppState_t;
AppState_t appState = UPDATE_TIME;

void s1Pressed(void){
  buttonPressed = true;
}

static void shakeStuff(void){
  dieRolled = true;
}

void setup() {
  Serial.begin(115200);

  pinMode(S1, INPUT);
  attachInterrupt(digitalPinToInterrupt(S1), s1Pressed, FALLING);
  
  // put your setup code here, to run once:
  die.attachShakeCallback(&shakeStuff);
  die.begin();
}

void loop() {
  
  switch(appState){
    case UPDATE_TIME:{
       unsigned long currentMillis = millis();
       if(currentMillis - previousMillis >= POLL_INTERVAL){
        previousMillis = currentMillis;
        appState = CHECK_DIE_ROLL;
       }
    } break;
    
    case CHECK_DIE_ROLL:{
      if(dieRolled){
        noRollCounter = 0;
        rollDuration++;
        dieRolled = false;
        die.roll();
      }
      else{
        if(noRollCounter == 1){
          appState = ANIMATE_ROLL;
        }
        else{
          noRollCounter++;
          appState = UPDATE_TIME;
        }
      }
    } break;

    case ANIMATE_ROLL:{
      // roll animatition
      if(rollDuration>1){
        die.roll(rollDuration);
      }
      rollDuration = 0;
      appState = UPDATE_TIME;
    } break;

    default:{
      appState = UPDATE_TIME;
    } break;
  }

  if(buttonPressed){
    die.roll(10);
    // reset counters
    appState = UPDATE_TIME;
    buttonPressed = false;
  }
  
}