#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include <pins_arduino.h>
#endif


#include "AndroidApp.h"
#include <Zowi.h>

//-- This function returns true if another sample
//-- should be taken (i.e. the TS time has passed since
//-- the last sample was taken
void AndroidApp::checkActivity(Zowi zowi, int response, bool checkType)
{
    switch (response) {
        case 1:
            if (checkType)
                zowi.playGesture(ZowiSuperHappy);
            else {
                zowi.sing(S_happy_short);
                zowi.putMouth(correct);
            }
            break;
        case 2:
            if (checkType)
                zowi.playGesture(ZowiSad);
            else {
                zowi.putMouth(wrong);
                zowi.sing(S_confused);
            }
            break;
    }
}

//-- Attach an oscillator to a servo
//-- Input: pin is the arduino pin were the servo
//-- is connected
void AndroidApp::sonar(Zowi zowi, int sonarTone)
{
    switch (sonarTone) {
        case 1:
            zowi._tone(TONE_SLOW, 30, 1);
            break;
        case 2:
            zowi._tone(TONE_MEDIUM, 30, 1);
            break;
        case 3:
            zowi._tone(TONE_FAST, 30, 1);
            break;
    }
}

void AndroidApp::prepareMusic(Zowi zowi) {
    int footUp[4] = {90, 90, 90, 120};
    zowi._moveServos(1000, footUp);

    zowi.putMouth(five);
    delay(1000);
    zowi.putMouth(four);
    delay(1000);
    zowi.putMouth(three);
    delay(1000);
    // zowi.putMouth(two);
    // delay(1000);
    // zowi.putMouth(one);
    // delay(1000);
}

void AndroidApp::music(Zowi zowi, int T)
{
    Serial.println("muuuuusic");
    zowi.musicRhythm(T);
}

/*************************************/
/* Set the oscillator period, in ms  */
/*************************************/
void AndroidApp::grid()
{

};
