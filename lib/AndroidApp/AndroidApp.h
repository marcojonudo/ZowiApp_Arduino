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
    void checkActivity(Zowi zowi, int response, bool checkType = true);
    void sonar(Zowi zowi, int sonarTone);
    void prepareMusic(Zowi zowi);
    void music(Zowi zowi, int T);
    void grid();

  private:

};

#endif
