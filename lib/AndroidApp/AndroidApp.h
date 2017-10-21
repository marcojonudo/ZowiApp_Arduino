//--------------------------------------------------------------
//-- Oscillator.pde
//-- Generate sinusoidal oscillations in the servos
//--------------------------------------------------------------
//-- (c) Juan Gonzalez-Gomez (Obijuan), Dec 2011
//-- GPL license
//--------------------------------------------------------------
#ifndef AndroidApp_h
#define AndroidApp_h

#include <Zowi.h>

#define TONE_SLOW 800
#define TONE_MEDIUM 1000
#define TONE_FAST 1200

class AndroidApp
{
  public:
    void checkActivity(Zowi zowi, int response);
    void checkActivityMouth(Zowi zowi, int response);
    void sonar(Zowi zowi, int sonarTone);
    void music(Zowi zowi, int T, bool firstTime);
    void turn(Zowi zowi, int direction);
    void grid();

  private:

};

#endif
