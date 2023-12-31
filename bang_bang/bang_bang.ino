#include <hp_BH1750.h>
#include <Adafruit_MotorShield.h>

//define IR pins 
#define IR_PIN_LEFT A0
#define IR_PIN_RIGHT A1

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

hp_BH1750 lightSensor; 
int bright = 0;

void setup() {
  //setup board
  AFMS.begin();
  
  // set pins to input
  pinMode(IR_PIN_LEFT, INPUT);
  pinMode(IR_PIN_RIGHT, INPUT);

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
//  
  // get readings from IR sensors 
  int ir_left = digitalRead(IR_PIN_LEFT);
  int ir_right = digitalRead(IR_PIN_RIGHT);
// analog to digital conversion
//  if low, white ; if high, black 
//  if (ir_left < 50){
//    ir_left = 0;
//  }
//  else {
//    ir_left = 1;
//  }
//
//  if (ir_right < 50){
//    ir_right = 0;
//  }
//  else{
//    ir_right = 1;
//  }  


  //start signal 
  if (lux > 1000){
    POS_M1->setSpeed(255);
    POS_M1->run(FORWARD);
    POS_M2->setSpeed(255);
    POS_M2->run(FORWARD);
    NEG_M3->setSpeed(255);
    NEG_M3->run(FORWARD);
    NEG_M4->setSpeed(255);
    NEG_M4->run(FORWARD);
    delay(100); 
  }


  //bang bang control algorithm
  if (ir_left == 0 && ir_right == 0){
    //move forward
    POS_M1->setSpeed(255);
    POS_M1->run(FORWARD);
    POS_M2->setSpeed(255);
    POS_M2->run(FORWARD);
    NEG_M3->setSpeed(255);
    NEG_M3->run(FORWARD);
    NEG_M4->setSpeed(255);
    NEG_M4->run(FORWARD); 
  }
  else if (ir_left == 0 && ir_right == 1){
    //turn right
    POS_M2->setSpeed(255);
    POS_M2->run(FORWARD);
    NEG_M3->setSpeed(255);
    NEG_M3->run(FORWARD);
    NEG_M4->setSpeed(50);
    POS_M1->setSpeed(50);
  }
  else if (ir_right == 0 && ir_left == 1){
    //turn left
    POS_M1->setSpeed(255);
    POS_M1->run(FORWARD); 
    NEG_M4->setSpeed(255);
    NEG_M4->run(FORWARD); 
    NEG_M3->setSpeed(50);
    POS_M2->setSpeed(50);
  }
  else{
    //rev down and stop
      POS_M1->setSpeed(0);
      POS_M2->setSpeed(0);
      NEG_M3->setSpeed(0);
      NEG_M4->setSpeed(0);
  }
}
