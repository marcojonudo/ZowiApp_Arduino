//----------------------------------------------------------------
//-- Zowi basic firmware v2
//-- (c) BQ. Released under a GPL licencse
//-- 04 December 2015
//-- Authors:  Anita de Prado: ana.deprado@bq.com
//--           Jose Alberca:   jose.alberca@bq.com
//--           Javier Isabel:  javier.isabel@bq.com
//--           Juan Gonzalez (obijuan): juan.gonzalez@bq.com
//--           Irene Sanz : irene.sanz@bq.com
//-----------------------------------------------------------------
//-- Experiment with all the features that Zowi has!
//-----------------------------------------------------------------

#include <Servo.h>
#include <Oscillator.h>
#include <EEPROM.h>
#include <BatReader.h>
#include <US.h>
#include <LedMatrix.h>

//-- Library to manage external interruptions
#include <EnableInterrupt.h>

//-- Library to manage serial commands
#include <ZowiSerialCommand.h>
ZowiSerialCommand SCmd;  //The SerialCommand object

//-- Zowi Library
#include <Zowi.h>
Zowi zowi;  //This is Zowi!!

#include <AndroidApp.h>
AndroidApp androidApp; //Android app functions

//---------------------------------------------------------
//-- Configuration of pins where the servos are attached
/*
         ---------------
        |               |
        |     O   O     |
        |               |
 YR ==> |               | <== YL
         ---------------
            ||     ||
            ||     ||
            ||     ||
 RR ==>   -----   ------  <== RL
          -----   ------
*/

  #define PIN_YL 2 //servo[0]
  #define PIN_YR 3 //servo[1]
  #define PIN_RL 4 //servo[2]
  #define PIN_RR 5 //servo[3]
//---------------------------------------------------------

//---Zowi Buttons
#define PIN_SecondButton 6
#define PIN_ThirdButton 7


///////////////////////////////////////////////////////////////////
//-- Global Variables -------------------------------------------//
///////////////////////////////////////////////////////////////////

const char programID[]="SUPER_ZOWI"; //Each program will have a ID

const char name_fac='$'; //Factory name
const char name_fir='#'; //First name

//-- Movement parameters
int T = 1000;              //Initial duration of movement
int moveId = 0;            //Number of movement
int moveSize = 15;         //Asociated with the height of some movements

//---------------------------------------------------------
//-- Zowi has 5 modes:
//--    * MODE = 0: Zowi is awaiting
//--    * MODE = 1: Dancing mode!
//--    * MODE = 2: Obstacle detector mode
//--    * MODE = 3: Noise detector mode
//--    * MODE = 4: ZowiPAD or any Teleoperation mode (listening SerialPort).
//---------------------------------------------------------
volatile int MODE = 0; //State of zowi in the principal state machine.

volatile bool buttonPushed=false;  //Variable to remember when a button has been pushed
volatile bool buttonAPushed=false; //Variable to remember when A button has been pushed
volatile bool buttonBPushed=false; //Variable to remember when B button has been pushed

unsigned long refTimeMilis = 0;
unsigned long absoluteRefTimeMilis = 0;
unsigned long mouthTimeMillis = 0;

int randomDance=0;
int randomSteps=0;

bool obstacleDetected = false;
bool connectedToApp = false;

bool showMouths = true;
bool assembly = false;

#define NUMBER_OF_COLUMNS 5
#define NUMBER_OF_ROWS 5
#define REMOVE_RIGHT_BITS 0b00111110111110111110111110111110

int distance;
bool waitForCommands = false;
bool displayMouths = false;
bool asamblea = false;
int frecuencies[4] = {500, 700, 900, 800};
int mouths[4] = {20, 10, 11, 13};
int counter = 0;
int totalMouths[23] = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};

unsigned long matrixCode = 0;

bool connectWithAppMode = false;

///////////////////////////////////////////////////////////////////
//-- Setup ------------------------------------------------------//
///////////////////////////////////////////////////////////////////
void setup() {

    //Serial communication initialization
    Serial.begin(115200);

    pinMode(PIN_SecondButton,INPUT);
    pinMode(PIN_ThirdButton,INPUT);

    //Set the servo pins
    zowi.init(PIN_YL, PIN_YR, PIN_RL, PIN_RR, true);

    //Set a random seed
    randomSeed(analogRead(A6));

    //Interrumptions
    enableInterrupt(PIN_SecondButton, aButtonPushed, RISING);
    enableInterrupt(PIN_ThirdButton, bButtonPushed, RISING);
    // enableInterrupt(PIN_ThirdButton, thirdButtonPushed, RISING);

    // Setup callbacks for SerialCommand commands
    SCmd.addCommand("C", checkActivity);
    SCmd.addCommand("D", checkActivityMouth);
    SCmd.addCommand("E", checkActivityZowi);
    SCmd.addCommand("G", guideZowi);
    SCmd.addCommand("S", sonar);
    SCmd.addCommand("O", operations);
    SCmd.addCommand("M", music);
    SCmd.addCommand("T", turn);
    SCmd.addCommand("W", walkForward);
    SCmd.addCommand("B", walkBackward);
    SCmd.addCommand("R", grid);
    // SCmd.addCommand("O2", androidApp.operations);
    // SCmd.addCommand("G", androidApp.grid);
    SCmd.addDefaultHandler(receiveStop);

    //Zowi wake up!
    zowi.sing(S_connection);
    zowi.home();

    //Send Zowi name, programID & battery level.
    // requestName();
    // delay(50);
    requestProgramId();
    // delay(50);
    // requestBattery();

    //Checking battery
    ZowiLowBatteryAlarm();

    if(!buttonPushed){
        zowi.putMouth(happyOpen);
    }

    refTimeMilis = millis;
    absoluteRefTimeMilis = millis();
}



///////////////////////////////////////////////////////////////////
//-- Principal Loop ---------------------------------------------//
///////////////////////////////////////////////////////////////////
void loop() {
    unsigned long elapsedTimeMillis = millis();
    int diff = elapsedTimeMillis - refTimeMilis;
    //if (elapsedTimeMillis - absoluteRefTimeMilis > 1800000 && !stopMouths) {
    if (diff > 500) {
        if (buttonAPushed && buttonBPushed) {
            connectWithAppMode = !connectWithAppMode;
            buttonAPushed = false;
            buttonBPushed = false;
            Serial.println("Amboooos");
            zowi.putMouth(happyOpen);
        }
        // if (buttonAPushed) {
        //     Serial.println("buttonAPushed");
        //     showMouths = false;
        //     assembly = !assembly;
        //     buttonAPushed = false;
        //     zowi.putMouth(happyOpen);
        // } else if (buttonBPushed) {
        //     Serial.println("buttonBPushed");
        //     assembly = false;
        //     showMouths = !showMouths;
        //     buttonBPushed = false;
        //     zowi.putMouth(happyOpen);
        // }
        //
        // if (elapsedTimeMillis - mouthTimeMillis > 1000 && showMouths) {
        //     Serial.println("showMouths");
        //     int randomMouth = random(10, 24);
        //     zowi.putMouth(randomMouth);
        //     mouthTimeMillis = millis();
        // } else if (assembly) {
        //     Serial.println("assembly");
        //     zowi._tone(frecuencies[counter%4], 500, 0);
        //     zowi.putMouth(mouths[counter%4]);
        //     counter = counter + 1;
        // }
        //
        refTimeMilis = millis();

        switch (MODE) {
            //-- MODE 0 - Zowi is waiting for commands
            //---------------------------------------------------------
            case 0: {
                Serial.println("CASE 0!");
                if (connectWithAppMode) {
                    SCmd.readSerial();
                } else {
                    executeVoidConnectedMode(elapsedTimeMillis);
                }

                //If Zowi is moving yet
                if (zowi.getRestState() == false){
                    zowi.home();
                }
                break;
            }
            //-- MODE 1 - Check activity content using ultrasound sensor
            //---------------------------------------------------------
            case 1:
                // Serial.println("CASE 1!");

                distance = zowi.getDistance();

                if (distance < 10) {
                    sendFinalAck();
                    MODE = 0;
                }
                break;
            //-- MODE 2 - Guide Zowi with the hand
            //---------------------------------------------------------
            case 2:
                Serial.println("CASE 2!");

                if (!buttonAPushed) {
                    distance = zowi.getDistance();

                    if (distance < 10)
                        zowi.walk(1, 1200);
                    else if (distance > 10 && distance < 20)
                        zowi.walk(1, 1000);
                    else if (distance > 20 && distance < 30)
                        zowi.walk(1, 800);
                    else
                        zowi.home();
                }
                else {
                    zowi.playGesture(ZowiSuperHappy);
                    MODE = 0;
                }

                break;
            //-- MODE 3 - Noise detector mode
            //---------------------------------------------------------
            case 3: {
                break;
            }
            //-- MODE 4 - ZowiPAD or any Teleoperation mode (listening SerialPort)
            //---------------------------------------------------------
            case 4:
                break;
            default:
                MODE=4;
                break;
        }
    }
}


///////////////////////////////////////////////////////////////////
//-- Functions --------------------------------------------------//
///////////////////////////////////////////////////////////////////

void executeVoidConnectedMode(long elapsedTimeMillis) {
    if (buttonAPushed) {
        Serial.println("buttonAPushed");
        showMouths = false;
        assembly = !assembly;
        buttonAPushed = false;
        zowi.putMouth(happyOpen);
    } else if (buttonBPushed) {
        Serial.println("buttonBPushed");
        assembly = false;
        showMouths = !showMouths;
        buttonBPushed = false;
        zowi.putMouth(happyOpen);
    }

    if (elapsedTimeMillis - mouthTimeMillis > 1000 && showMouths) {
        Serial.println("showMouths");
        int randomMouth = random(10, 24);
        zowi.putMouth(randomMouth);
        mouthTimeMillis = millis();
    } else if (assembly) {
        Serial.println("assembly");
        zowi._tone(frecuencies[counter%4], 500, 0);
        zowi.putMouth(mouths[counter%4]);
        counter = counter + 1;
    }

    //refTimeMilis = millis();
}

//-- Function executed when A button is pushed
void aButtonPushed() {
    buttonAPushed=!buttonAPushed;
    if (!buttonAPushed) {
        zowi.putMouth(happyClosed);
    }
}

void bButtonPushed() {
    buttonBPushed = !buttonBPushed;
}


//-- Function to receive Stop command.
void receiveStop(){

    sendAck();
    zowi.home();
    sendFinalAck();

}

void checkActivity() {
    //sendAck & stop if necessary
    sendAck();
    zowi.home();

    int responseType;

    // Bluetooth commands: C 1, C 2
    char *arg;
    arg = SCmd.next();

    if (arg != NULL) {
        responseType = atoi(arg);
        androidApp.checkActivity(zowi, responseType);
    }
    else {
        zowi.putMouth(interrogation);
        delay(2000);
        zowi.putMouth(happyOpen);
    }

    /* Send final ACK is not necessary as Android app assumes correct reception */
}

void checkActivityMouth() {
    sendAck();
    zowi.home();

    int responseType;

    //Bluetooth commands: D 1, D 2
    char *arg;
    arg = SCmd.next();

    if (arg != NULL) {
        responseType = atoi(arg);
        androidApp.checkActivityMouth(zowi, responseType);
    }
    else {
        zowi.putMouth(interrogation);
    }
    delay(2000);
    zowi.putMouth(happyOpen);
}

void checkActivityZowi() {
    sendAck();
    zowi.home();

    Serial.println("checkActivityZowi");

    MODE = 1;
}

void sonar() {
    //sendAck & stop if necessary
    sendAck();
    zowi.home();

    int sonarTone;

    char *arg;
    arg = SCmd.next();

    if (arg != NULL) {
        sonarTone = atoi(arg);
        androidApp.sonar(zowi, sonarTone);
    }
    else {
        zowi.putMouth(interrogation);
        delay(4000);
        zowi.clearMouth();
    }

    sendFinalAck();
}

void operations() {
    zowi.home();

    char *arg = SCmd.next();

    zowi.clearMouth();
    unsigned long row;
    int bitshift;
    while (arg != NULL) {
        bitshift = 24;
        for (int i=0; i<5; i++) {
            row = arg[i] - '0';
            matrixCode = matrixCode | row << bitshift;
            bitshift = bitshift - 6;
        }
        zowi.putMouth(matrixCode, false);
        delay(500);

        matrixCode = matrixCode << 1 & REMOVE_RIGHT_BITS;
        zowi.putMouth(matrixCode, false);

        arg = SCmd.next();
    }

    MODE = 0;
    sendFinalAck();
}

void music() {
    sendAck();
    zowi.home();

    char *arg = SCmd.next();

    int period;
    bool firstTime = true;
    while (arg != NULL) {
        period = atof(arg) * 2000;
        androidApp.music(zowi, period, firstTime);

        if (firstTime)
            firstTime = false;

        arg = SCmd.next();
    }

    zowi.putMouth(happyOpen);
    MODE = 0;
}

void turn() {
    sendAck();
    zowi.home();

    char *arg = SCmd.next();

    if (arg != NULL) {
        int direction = atoi(arg);
        Serial.print("turn: "); Serial.println(direction);
        androidApp.turn(zowi, direction);
    }

    zowi.home();
    zowi.putMouth(happyOpen);

    MODE = 0;
}

void walkForward() {
    zowi.walk(3);
}

void walkBackward() {
    zowi.walk(3, 1000, -1);
}

void grid() {
    zowi.home();

    char *arg = SCmd.next();

    while (arg != NULL) {
        int direction = atoi(arg);
        androidApp.grid(zowi, direction);

        arg = SCmd.next();
    }

    zowi.home();
    zowi.putMouth(happyOpen);

    MODE = 0;
}

void guideZowi() {
    sendAck();
    zowi.home();

    MODE = 2;
}


//-- Function to send Zowi's name
void requestName(){

    zowi.home(); //stop if necessary

    char actualZowiName[11]= "";  //Variable to store data read from EEPROM.
    int eeAddress = 5;            //EEPROM address to start reading from

    //Get the float data from the EEPROM at position 'eeAddress'
    EEPROM.get(eeAddress, actualZowiName);

    Serial.print(F("&&"));
    Serial.print(F("E "));
    Serial.print(actualZowiName);
    Serial.println(F("%%"));
    Serial.flush();
}


//-- Function to send program ID
void requestProgramId(){

    zowi.home();   //stop if necessary

    Serial.print(F("&&"));
    Serial.print(F("I "));
    Serial.print(programID);
    Serial.println(F("%%"));
    Serial.flush();
}


//-- Function to send Ack comand (A)
void sendAck(){

  delay(30);

  Serial.print(F("&&"));
  Serial.print(F("A"));
  Serial.println(F("%%"));
  Serial.flush();
}


//-- Function to send final Ack comand (F)
void sendFinalAck(){

  delay(30);

  Serial.print(F("&&"));
  Serial.print(F("F"));
  Serial.println(F("%%"));
  Serial.flush();
}



//-- Functions with animatics
//--------------------------------------------------------

void ZowiLowBatteryAlarm(){

    double batteryLevel = zowi.getBatteryLevel();

    if(batteryLevel<45){

        while(!buttonPushed){

          zowi.putMouth(thunder);
          zowi.bendTones (880, 2000, 1.04, 8, 3);  //A5 = 880

          delay(30);

          zowi.bendTones (2000, 880, 1.02, 8, 3);  //A5 = 880
          zowi.clearMouth();
          delay(500);
      }
    }
}
