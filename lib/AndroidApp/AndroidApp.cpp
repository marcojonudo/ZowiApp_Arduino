#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include <pins_arduino.h>
#endif


#include "AndroidApp.h"
#include <Zowi.h>

//-- Function to react to an activity check using predefined gestures
void AndroidApp::checkActivity(Zowi zowi, int response)
{
    zowi.setRestState(false);
    switch (response) {
        /* Correct answer */
        case 1:
            zowi.playGesture(ZowiSuperHappy);
            break;
        /* Wrong answer */
        case 0:
            zowi.playGesture(ZowiSad);
            break;
    }
    zowi.home();
}

//-- Function to react to an activity check using mouth shapes and sounds
void AndroidApp::checkActivityMouth(Zowi zowi, int response) {
    zowi.setRestState(false);
    switch (response) {
        case 1:
            zowi.sing(S_happy_short);
            zowi.putMouth(correct);
            delay(1000);
            break;
        case 0:
            zowi.sing(S_confused);
            zowi.putMouth(wrong);
            delay(1000);
            break;
    }
    zowi.home();
}

//-- Makes Zowi act as a sonar, emitting bips with a
//-- variable frecuency
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

//-- Counts from 5 to 0 to prepare for rhytm movements
void AndroidApp::music(Zowi zowi, int T, bool firstTime)
{
    zowi.setRestState(false);
    zowi.musicRhythm(T, firstTime);
    zowi.home();
}

//-- Zowi turns 90º left or reight depending on the received parameter
void AndroidApp::turn(Zowi zowi, int direction)
{
    zowi.setRestState(false);
    zowi.turnInPlace(direction);
    zowi.home();
}

//-- Walks straight forwared or turn depending on the parameter
void AndroidApp::grid(Zowi zowi, int direction) {
    switch (direction) {
        case 0:
            zowi.walk(3);
            break;
        default:
            zowi.turnInPlace(direction);
            break;
    }
};
