//LIBRARIES
#include <DualMC33926MotorShield.h>
#include <Triskar.h>
#include <Encoder.h>
#include <NewPing.h>
#include <Filters.h>
#include <FastRunningMedian.h>
#include <Adafruit_NeoPixel.h>
#include <CapacitiveSensor.h>
#include <DFRobotDFPlayerMini.h>

#define TIME_TO_SETUP 9000

//FOTORESISTOR
#define FOTORES_PIN A3
int fotores_value;

//VOLTAGE CHECKER
#define NUM_SAMPLES 10
#define VOLT_CHECKER_PIN A4
int sum = 0;                    // sum of samples taken
unsigned char sample_count = 0; // current sample number
float voltage = 0.0;            // calculated voltage
unsigned long int lastWarning=0;//last time warning advice

//SOUND VARIABLES
boolean firstSound=false;
int playI = 0;
DFRobotDFPlayerMini myDFPlayer;
#define BUSY_PIN 5

//GAME VARIABLES
enum gameStates {no_game, setting, make_question, wait_answer, right_answer,wrong_answer,end_game,mov};
gameStates gameState = no_game;
#define colorGame 1
#define animalGame 2
int gameNumber = 0;
#define timePerAnsw 50000
#define questionsPerEx 10
int questionI = 0;
#define max_q_repeat 5
int repeatQuestionCounter = 0;
int gameAnswers[][questionsPerEx] = {{}, {0, 2, 3, 0, 1, 3, 2, 2, 1, 0}, {0, 2, 1, 2, 2, 3, 1, 0, 2, 3}};
//0-blu 1-giallo 2-verde 3-rosso
unsigned long int startWaitingTime = 0;
boolean questionResult[questionsPerEx];
float questionTime[questionsPerEx];
int tries[questionsPerEx];


//BODY CAPACITIVES
#define BODY_CAPACITIVE_COMMONPIN 49
#define BODY_SX_PIN 47
#define BODY_DX_PIN 45
#define BODY_FRONT_PIN 41
#define BODY_BEHIND_PIN 43
const int bodyNumberSensors = 4;
CapacitiveSensor* bodySensor[bodyNumberSensors];
long bodySensorValue[bodyNumberSensors];
int bodyThreshold = 500;
int behindBodyThreshold = 3500;
boolean leftCapacitive = false;
boolean rightCapacitive = false;
boolean frontCapacitive = false;
boolean behindCapacitive = false;


//HEAD CAPACITIVES
const int headNumberSensors = 4;
CapacitiveSensor* headSensor[headNumberSensors];
long headSensorValue[headNumberSensors];
int headThreshold = 500;
int headSpecialThreshold = 3500;
int pressedButton = -1;

//FRONT LEDS PINS, CONSTANT AND VARIABLES
#define FRONT_LEDPIN 9
#define FRONT_LED_NUMBER 9
Adafruit_NeoPixel head_strip = Adafruit_NeoPixel(FRONT_LED_NUMBER, FRONT_LEDPIN, NEO_GRB + NEO_KHZ800);
//states
enum ledStates {led_off, rainbow_cycle, color_wipe, color_pulse};
ledStates led_state=rainbow_cycle;
uint32_t head_color;
const uint32_t blue = head_strip.Color(0, 0, 255);
const uint32_t red = head_strip.Color(255, 0, 0);
const uint32_t green = head_strip.Color(0, 255, 0);
const uint32_t light_blue = head_strip.Color(0, 255, 255);
const uint32_t violet = head_strip.Color(255, 0, 255);
const uint32_t yellow = head_strip.Color(255, 170, 0);
const uint32_t orange = head_strip.Color(255, 100, 0);

//MICRO PINS, CONSTANT AND VARIABLES
#define soundPin  A11 //sound sensor attach to A11
#define microISequence 200
#define microISequenceShortMin 10
#define microISequenceShortMax 40
#define microSoglia 120.00f
int microI = 0;
float microFilterFrequency = 1.0;
FilterOnePole microLowpassFilter( LOWPASS, microFilterFrequency );
unsigned long int lastadd=0;
unsigned long int movementFinishTime=TIME_TO_SETUP+1000;

//MOTORS AND ENCODERS PINS AND OBJECTS
//-------SHIELD 1-------
#define nD2m1 48
#define nSFm1 52
//--------Motor 1-----
#define M1DIR 50
#define M1PWM 4
#define M1FB A7

//-------SHIELD 2-------
#define nD2m23 10
#define nSFm23 11
//--------Motor 2-----
#define M2DIR 46
#define M2PWM 12 //17
#define M2FB A6
//--------Motor 3-----
#define M3DIR 40
#define M3PWM 13 //16
#define M3FB A5

//-------ENCODER 1-------
#define EncA1 2
#define EncB1 3
//-------ENCODER 2-------
#define EncA2 18
#define EncB2 19
//-------ENCODER 3-------
#define EncA3 21
#define EncB3 20

Encoder posReader1(EncA1, EncB1), posReader2(EncA2, EncB2), posReader3(EncA3, EncB3);
DualMC33926MotorShield m1(M1DIR, M1PWM, nD2m1, nSFm1);
DualMC33926MotorShield m2m3(M2DIR, M2PWM, M3DIR, M3PWM, nD2m23, nSFm23);
Triskar triskar(m1, m2m3, posReader1, posReader2, posReader3);

//PRINT-TIMING VARIABLE
unsigned long lastMilliPrint = 0;

// MOVEMENTS VARIABLES
boolean move = false;
#define no_movement   0
#define follow        1
#define autonomous_movement 2
#define make_eight    3
#define turn180r      4  //rotazione 180 gradi USANDO RUOTA DESTRA COME CENTRO DI ROTAZIONE
#define turn180l      5  //rotazione 180 gradi USANDO RUOTA SINISTRA COME CENTRO DI ROTAZIONE
#define turnAlphaR    6  //rotazione di alpha(variabile globale) gradi a destra USANDO IL CENTRO DEL ROBOT COME CENTRO DI ROTAZIONE
#define turnAlphaL    7
#define makeOnem      8
#define make_circle   9
#define scared_round  10
#define dontwonna     11
#define scared_behind 12
#define make_happy0   13
#define make_happy1   14
#define make_happy2   15
#define make_happy3   16
#define make_sad0     17
#define make_sad1     18
#define make_sad2     19
#define angrymov      20

double alpha = 0;
byte next_movement=make_circle;
byte actual_movement = no_movement;
byte prec_movement = no_movement;
boolean follow2 = false;
boolean aut_mov = false;
boolean makeInverse = false;
int movementI = 0;
unsigned long int randomTurnTime = 15000 + rand() % (20000);
unsigned long int lastObstacleTime = 0;
int obstacleCount = 0;
int dir = rand() % 2;

long int movStartTime = 0;
double startPosX = 0;
double startPosY = 0;
double startPosTh = 0;

double speed_trg = 0;

//BLUETOOTH
//buttons
boolean triangolo;
boolean quadrato;
boolean cerchio;
boolean croce;
boolean startbutton;
boolean select;
char b = ' ';
//interpreter states
enum btStates {choose_modality, choose_game, sg_waiting, game_modality, fam_modality, discharge};
btStates interpreterState = choose_modality;

//---------SONARS---------
//-----constants-----
#define SONAR_NUM     4 // Number or sensors.
#define MAX_DISTANCE 400 // Maximum distance (in cm) to ping.
#define PING_INTERVAL 66 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).
#define FAR_DISTANCE 200.0f
#define CLOSE_DISTANCE 70.0f
#define VERYCLOSE_DISTANCE 50.0f
#define MEDIAN_NUMBER 7
#define FILTERFREQUENCY 1.0f        //PB filter frequency
#define COUNTER 50                  //when targetPos reach COUNTER(right) or -COUNTER(left) the robot is sure about where obstacle is(that's for avoid sonars false readings)
#define MAX_COUNTER 2*COUNTER             //maximum COUNTER value
//-----variables-----
unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
unsigned int cm[SONAR_NUM];         // Where the ping distances are stored.
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.

float f_front = 400;                //filtered values
float f_right = 400;
float f_left = 400;
float f_back = 400;

int backI = 0;
int targetPos = 0;                  //variable for suppose where the person to follow/obstacle is. Positive is right, negative is left.
float actual_distance = 400;        //viariable for store the distance of the closest object saw by sonars
enum obstacle {left, right, front, none};
obstacle last_obstacle = none;      //variable to define the last sonar that saw an obstacle
obstacle actual_obstacle = none;    //variable to define the sonar that is seeing the closest thing
//-----filters-----
FastRunningMedian<unsigned int, MEDIAN_NUMBER, 400> frontMedian, rightMedian, leftMedian, backMedian;
FilterOnePole front_sonar_f( LOWPASS, FILTERFREQUENCY );
FilterOnePole left_sonar_f( LOWPASS, FILTERFREQUENCY );
FilterOnePole right_sonar_f( LOWPASS, FILTERFREQUENCY );
FilterOnePole back_sonar_f( LOWPASS, FILTERFREQUENCY );
//-----objects-----
NewPing sonar[SONAR_NUM] = {     // Sensor object array.
  NewPing(30, 28, MAX_DISTANCE), // Each sensor's trigger pin, echo pin, and max distance to ping.
  NewPing(26, 24, MAX_DISTANCE),
  NewPing(34, 32, MAX_DISTANCE),
  NewPing(36, 25, MAX_DISTANCE)
};
//-----booleans-----
boolean front_obstacle = false;
boolean right_obstacle = false;
boolean left_obstacle = false;
boolean back_obstacle = false;
boolean close_front_obstacle = false;
boolean close_right_obstacle = false;
boolean close_left_obstacle = false;
boolean close_back_obstacle = false;
boolean veryclose_front_obstacle = false;
boolean veryclose_right_obstacle = false;
boolean veryclose_left_obstacle = false;
boolean veryclose_back_obstacle = false;
boolean no_obstacle = true;



void setup() {
  Serial.begin(115200);
  Serial.println("Arduino is ready");
  //serial.println("Remember to select Both NL & CR in the serial monitor");
  srand(millis());
  bodyCapacitiveSetup();
  dfPlayerSetup();
  voltageCheckSetup();
  fotoresSetup();
  sonarSetup();
  ledSetup();
  headLedRainbow();
  //historyPosX[0]=posX;
  //historyPosY[0]=posY;
  //historyIndex++;
  // HC-05 default serial speed for AT mode is 38400
  Serial3.begin(38400);
  Serial3.println("Are YOU ready??");
  

}


void print()  {                                                      // display data
  if ((millis() - lastMilliPrint) >= 50)   {
    lastMilliPrint = millis();


    /*    Serial.print("DONTWONNA:  ");
        Serial.println(movementI);
        Serial.print("MOV START TIME: ");
        Serial.println(movStartTime);
        Serial.print("START POS TH: ");
        Serial.println(startPosTh);
    */
    /*Serial.print("SP:  ");     Serial.print(speed_req[0]); Serial.print("    "); Serial.print(speed_req[1]); Serial.print("    "); Serial.println(speed_req[2]);
      Serial.print("RPM: ");     Serial.print(speed_act[0]); Serial.print("    "); Serial.print(speed_act[1]); Serial.print("    "); Serial.println(speed_act[2]);
      Serial.print("PWM: ");     Serial.print(PWM_val[0]);  Serial.print("   "); Serial.print(PWM_val[1]);   Serial.print("    "); Serial.println(PWM_val[2]);
      Serial.print("POS: ");   Serial.print(posReader1.read());Serial.print("    "); Serial.print(posReader2.read());Serial.print("    "); Serial.println(posReader3.read());
    */
    /*
      if(targetPos==0) Serial.println("FRONT");
      else if (targetPos<0) Serial.println("LEFT");
      else if (targetPos>0) Serial.println("RIGHT");
    */
    /*
        Serial.print("RIGHT: ");
        Serial.print(f_right);
        Serial.print("  CENTER: ");
        Serial.print(f_front);
        Serial.print("  LEFT: ");
        Serial.print(f_left);
        Serial.print("  BACK: ");
        Serial.println(f_back);
      */
      /*
      //    Serial.print("  TargetPos:  ");
      //    Serial.print(targetPos);
      //    Serial.print("  Actual Obstacle:  ");
      //    switch(actual_obstacle){
      //      case 0: Serial.print("left");break;
      //      case 1: Serial.print("right");break;
      //      case 2: Serial.print("front");break;
      //      case 3: Serial.print("none");break;
      //    }
      //    Serial.print("  Last Obstacle:  ");
      //    switch(last_obstacle){
      //      case 0: Serial.println("left");break;
      //      case 1: Serial.println("right");break;
      //      case 2: Serial.println("front");break;
      //      case 3: Serial.println("none");break;
      //    }


      /*    Serial.print("  BACK: ");
        Serial.println(cm[3]);
      /*
      //Serial.print("triskar PosX  "); Serial.println(triskar.getPosX());
      //Serial.print("target: "); Serial.println(startPosX+100.0f);
    
    Serial.println(voltage);

    Serial.println(fotores_value);
    
    /*Serial.print("millis()-movementFinishTime:  "); Serial.println(millis()-movementFinishTime);

    Serial.print("actual_movement  "); Serial.println(actual_movement);
    Serial.print("digitalRead(BUSY_PIN)  "); Serial.println(digitalRead(BUSY_PIN));
    Serial.print("gameState  "); Serial.println(gameState);
    Serial.print("triskar.isStopped()  "); Serial.println(triskar.isStopped());*/   
/*    Serial.print("micro val: "); Serial.println(microLowpassFilter.output());
    Serial.print("microI: "); Serial.println(microI);
*/
      
    /*  Serial.print("actual_movement:  "); Serial.println(actual_movement);
      Serial.print("myDFPlayer.available():  "); Serial.println(myDFPlayer.available());
     /* Serial.print("gameState:  "); Serial.println(gameState);
      

    //Serial.print("led val: "); Serial.println(ledI);*/
    /*Serial.print("obstacleCount: "); Serial.print(obstacleCount);
      //Serial.print("  millis: "); Serial.print(millis());
      Serial.print("  millis - mov Start Time: "); Serial.print(millis()-movStartTime);
      Serial.print("  rand turn Time: "); Serial.print(randomTurnTime);
      Serial.print("lastObstacleTime"); Serial.println(lastObstacleTime);
      //millis()-movStartTime>=randomTurnTime || movementI>=5
      /*Serial.print("movement I2: "); Serial.println(movementI2);

      Serial.print("actual pos: "); Serial.println(triskar.getPosTh());
      /*Serial.print("target pos: "); Serial.println(startPosTh+PI/12.0);

      Serial.print(gameState);Serial.print("    Pressed Button: ");Serial.println(pressedButton);
    
    Serial.print("Actual_movement:   "); Serial.print(actual_movement); Serial.print("  prec_movement:   "); Serial.println(prec_movement);
*/


    /*Serial.print(millis()); Serial.print(",");
      Serial.print(speedX); Serial.print(",");
      Serial.print(speedY); Serial.print(",");
      Serial.print(speedTh); Serial.print(",");*/

    /*Serial3.print(millis()); Serial3.print(",");
      Serial3.print(speedX); Serial3.print(",");
      Serial3.print(speedY); Serial3.print(",");
      Serial3.print(speedTh); Serial3.print(",");*/

    /*
        Serial.print(posX); Serial.print(",");
        Serial.print(posY); Serial.print(",");
        Serial.print(posTh);Serial.print(",");
        Serial.println(k);
    */

    /*Serial3.print(posX); Serial3.print(",");
      Serial3.print(posY); Serial3.print(",");
      Serial3.print(posTh);Serial3.print(",");
      Serial3.println(k);*/
  }
}



void pidLoop() {
  if (move) {
    triskar.PIDLoop();
  }
}

void loop() {
  //sensori
  //bodyCapacitiveLoop2();
  voltageCheckloop();
  sonarLoop();
  fotoresLoop();
  microLoop();
  //FirstSound();
  btInterpreter();
  //attuatori
  pidLoop();
  makeMovement();
  headLedLoop();
  gameModality();
  //InversePath();
  //printMotorInfo();
  print();
}

