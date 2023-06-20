#include <hp_BH1750.h>
#include <Adafruit_MotorShield.h>

//define IR pins 
#define IR_L A0
#define IR_M A1
#define IR_R A2

//define motor pins
#define POS_M1_TERMINAL 1
#define POS_M2_TERMINAL 2
#define NEG_M3_TERMINAL 3
#define NEG_M4_TERMINAL 4

// instantiating motorshield
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Accessing motors form shield
Adafruit_DCMotor *POS_M1 =AFMS.getMotor(POS_M1_TERMINAL);
Adafruit_DCMotor *POS_M2 =AFMS.getMotor(POS_M2_TERMINAL);
Adafruit_DCMotor *NEG_M3 = AFMS.getMotor(NEG_M3_TERMINAL);
Adafruit_DCMotor *NEG_M4 = AFMS.getMotor(NEG_M4_TERMINAL);

// light sensor initialization 
hp_BH1750 lightSensor; 
int bright = 0;

//define speeds 
const int high = 255;
const int mod = 100; 
const int low = 25;


void setup() {
  //setup board
  AFMS.begin();
  
  // set pins to input
  pinMode(IR_L, INPUT);
  pinMode(IR_M, INPUT);
  pinMode(IR_R, INPUT);

  //reset motors and set to output
  POS_M1->setSpeed(0);
  POS_M1->run(RELEASE);
  POS_M2->setSpeed(0);
  POS_M2->run(RELEASE);
  
  NEG_M3->setSpeed(0);
  NEG_M3->run(RELEASE);
  NEG_M4->setSpeed(0);
  NEG_M4->run(RELEASE);

//  //initialize sensor with address
  lightSensor.begin(BH1750_TO_GROUND);
}

void loop() {
  // get readings from light sensor 
  lightSensor.start();
  int lux = lightSensor.getLux();

  //get readings from ir sensors, store in array
  int readings[] = {analogRead(IR_L), analogRead(IR_M), analogRead(IR_R)};

// if low, white ; if high, black - conv analog to digital
  for (int i = 0; i < 3; i++){
    if (readings[i] < 50){
    readings[i] = 0;
    } 
    else {
    readings[i] = 1;
    }
  }

  //start signal -- if this doens't work, remove while loop and this conditional - use the bang-bang starter
  if (lux > 1000){
    bright = 1; 
  }

  while (bright){

  //Case I: [0 0 0]
  if (readings[0] == 0 && readings[1] == 0 && readings[2] == 0){
    //rotate CW until left or right sensors detect black
    //back wheel turns faster than front
    POS_M1->setSpeed(high);
    POS_M1->run(FORWARD); 
    NEG_M4->setSpeed(high);
    NEG_M4->run(FORWARD); 
    NEG_M3->setSpeed(0);
    POS_M2->setSpeed(0);
  }
  //Case II: [1 0 0]
  else if (readings[0] == 1 && readings[1] == 0 && readings[2] == 0){
    //sharp turn left
    POS_M1->setSpeed(high);
    POS_M1->run(FORWARD); 
    NEG_M4->setSpeed(high);
    NEG_M4->run(FORWARD); 
    NEG_M3->setSpeed(low);
    NEG_M3->run(BACKWARD);
    POS_M2->setSpeed(low);
    POS_M2->run(BACKWARD);

  }

  //Case III: [0 1 0]
  else if (readings[0] == 0 && readings[1] == 1 && readings[2] == 0){
    //go straight
    POS_M1->setSpeed(high);
    POS_M1->run(FORWARD); 
    NEG_M4->setSpeed(high);
    NEG_M4->run(FORWARD); 
    NEG_M3->setSpeed(high);
    NEG_M3->run(FORWARD);
    POS_M2->setSpeed(high);
    POS_M2->run(FORWARD);
  }

  //Case IV: [0 0 1]
  else if (readings[0] == 0 && readings[1] == 0 && readings[2] == 1){
    //sharp turn right
    POS_M2->setSpeed(high);
    POS_M2->run(FORWARD);
    NEG_M3->setSpeed(high));
    NEG_M3->run(FORWARD);
    NEG_M4->setSpeed(low);
    NEG_M4->run(BACKWARD);
    POS_M1->setSpeed(low);
    POS_M1->run(BACKWARD);
  }

  //Case V: [1 1 0]
  else if (readings[0] == 1 && readings[1] == 1 && readings[2] == 0){
    //slow turn left - rev up 
    for (int i = low; i < high; i+=25){
      POS_M1->setSpeed(i);
      POS_M1->run(FORWARD); 
      NEG_M4->setSpeed(i);
      NEG_M4->run(FORWARD); 
      // // (if want inside wheels to turn opposite way, uncomment this section)
      // NEG_M3->setSpeed(low);
      // NEG_M3->run(FORWARD);
      // POS_M2->setSpeed(low);
      // POS_M2->run(FORWARD); 
    }

  //Case VI: [0 1 1 ]
  else if (readings[0] == 0 && readings[1] == 1 && readings[2] == 1){
    // slow turn right - rev up 
    for (int i = low; i < high; i+=25){
      POS_M2->setSpeed(i);
      POS_M2->run(FORWARD); 
      NEG_M3->setSpeed(i);
      NEG_M3->run(FORWARD); 
      // // (if want inside wheels to turn opposite way, uncomment this section)
      // NEG_M4->setSpeed(low);
      // NEG_M4->run(FORWARD);
      // POS_M1->setSpeed(low);
      // POS_M1->run(FORWARD); 
    }
  }

  //Case VII: [1 0 1] || [1 1 1]
  else{
    //stpp
    POS_M1->setSpeed(0);
    NEG_M4->setSpeed(0); 
    NEG_M3->setSpeed(0);
    POS_M2->setSpeed(0);
  }  
  }
}
