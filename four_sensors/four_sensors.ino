#include <hp_BH1750.h>
#include <Adafruit_MotorShield.h>

//define IR pins 
#define IR_L A0
#define IR_M A1
#define IR_M_UP A2
#define IR_R A3

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
  pinMode(IR_M_UP, INPUT);
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
 
  // get readings from IR sensors 
  int readings[] = {analogRead(IR_L), analogRead(IR_M), analogRead(IR_M_UP), analogRead(IR_R)};

// if low, white ; if high, black - conv analog to digital
  for (int i = 0; i < 4; i++){
    if (readings[i] < 50){
    readings[i] = 0;
    } 
    else {
    readings[i] = 1;
    }
  }


  //start signal 
  if (lux > 1000){
    bright = 1; 
  }

  while (bright){

  //Case I: [0 0 0 0]
  if (readings[0] == 0 && readings[1] == 0 && readings[2] == 0 && readings[3] == 0){
    //rotate CW and move forward a little until left or right sensors detect black
    //back wheel turns faster than front
    POS_M1->setSpeed(high);
    POS_M1->run(FORWARD); 
    NEG_M4->setSpeed(high);
    NEG_M4->run(FORWARD); 
    NEG_M3->setSpeed(low);
    NEG_M3->run(FORWARD);
    POS_M2->setSpeed(low);
    POS_M2->run(FORWARD);
  }
  //Case II: [1 0 0 0]
  if (readings[0] == 1 && readings[1] == 0 && readings[2] == 0 && readings[3] == 0){
    //sharp turn left
    //can toggle this depending on performance - may need to set speeds to 0 of negM3 and posm2
    POS_M1->setSpeed(high);
    POS_M1->run(FORWARD); 
    NEG_M4->setSpeed(high);
    NEG_M4->run(FORWARD); 
    NEG_M3->setSpeed(low);
    NEG_M3->run(BACKWARD);
    POS_M2->setSpeed(low);
    POS_M2->run(BACKWARD);
  }

  //Case III: [0 1 0 0]
  if (readings[0] == 0 && readings[1] == 1 && readings[2] == 0 && readings[3] == 0){
    //go straight, but slow down as likely approaching curve
    for (int i = high; i > low; i-=15){
        POS_M1->setSpeed(i);
        POS_M1->run(FORWARD); 
        NEG_M4->setSpeed(i);
        NEG_M4->run(FORWARD); 
        NEG_M3->setSpeed(i);
        NEG_M3->run(FORWARD);
        POS_M2->setSpeed(i);
        POS_M2->run(FORWARD);
    }
    
  }

  //Case IV: [0 0 1 0]
  if (readings[0] == 0 && readings[1] == 0 && readings[2] == 1 && readings[3] == 0) {
      // keep going straight
        POS_M1->setSpeed(high);
        POS_M1->run(FORWARD); 
        NEG_M4->setSpeed(high);
        NEG_M4->run(FORWARD); 
        NEG_M3->setSpeed(high);
        NEG_M3->run(FORWARD);
        POS_M2->setSpeed(high);
        POS_M2->run(FORWARD);
    }

  //Case V: [0 0 0 1]
  if (readings[0] == 0 && readings[1] == 0 && readings[2] == 0 && readings[3] == 1){
      //sharp turn right 
      POS_M1->setSpeed(high);
      POS_M1->run(FORWARD); 
      NEG_M4->setSpeed(high);
      NEG_M4->run(FORWARD); 
      NEG_M3->setSpeed(low);
      NEG_M3->run(BACKWARD);
      POS_M2->setSpeed(low);
      POS_M2->run(BACKWARD);
    }

  //Case VI: [1 1 0 0]
if (readings[0] == 1 && readings[1] == 1 && readings[2] == 0 && readings[3] == 0) {
    // slow turn left
    for (int i = low; i < high; i += 25) {
    POS_M1->setSpeed(i);
    POS_M1->run(FORWARD); 
    NEG_M4->setSpeed(i);
    NEG_M4->run(FORWARD); 
    // (If you want the inside wheels to not turn opposite way, uncomment this section)
    NEG_M3->setSpeed(low);
    NEG_M3->run(FORWARD);
    POS_M2->setSpeed(low);
    POS_M2->run(FORWARD); 
    }
}

  // Case VII: [1 0 1 0]
  if (readings[0] == 1 && readings[1] == 0 && readings[2] == 1 && readings[3] == 0) {
      //sharper left
      POS_M1->setSpeed(high);
      POS_M1->run(FORWARD); 
      NEG_M4->setSpeed(high);
      NEG_M4->run(FORWARD); 
      //can toggle to 0 as well 
      NEG_M3->setSpeed(low);
      NEG_M3->run(FORWARD);
      POS_M2->setSpeed(low);
      POS_M2->run(FORWARD);
    }

    // Case VIII: [0 1 1 0]
if (readings[0] == 0 && readings[1] == 1 && readings[2] == 1 && readings[3] == 0){
      //sharper left
      POS_M1->setSpeed(mod);
      POS_M1->run(FORWARD); 
      NEG_M4->setSpeed(mod);
      NEG_M4->run(FORWARD); 
      //can toggle to 0 as well 
      NEG_M3->setSpeed(low);
      NEG_M3->run(FORWARD);
      POS_M2->setSpeed(low);
      POS_M2->run(FORWARD);
    }
    
    // Case IX: [0 1 1 1]
  if(readings[0] == 0 && readings[1] == 1 && readings[2] == 1 && readings[3] == 1){
        //slow right turn
      POS_M2->setSpeed(high);
      POS_M2->run(FORWARD); 
      NEG_M3->setSpeed(high);
      NEG_M3->run(FORWARD); 
      NEG_M4->setSpeed(low);
      NEG_M4->run(FORWARD);
      POS_M1->setSpeed(low);
      POS_M1->run(FORWARD);
    }

    // Case X: [1 1 1 0]
  if(readings[0] == 1 && readings[1] == 1 && readings[2] == 1 && readings[3] == 0){
        //slow left turn 
        POS_M2->setSpeed(low);
      POS_M2->run(FORWARD); 
      NEG_M3->setSpeed(low);
      NEG_M3->run(FORWARD); 
      NEG_M4->setSpeed(high);
      NEG_M4->run(FORWARD);
      POS_M1->setSpeed(high);
      POS_M1->run(FORWARD);
    }

    // Case XI: [1 1 0 1]
  if(readings[0] == 1 && readings[1] == 1 && readings[2] == 0 && readings[3] == 1){
      //go straight but slower
      POS_M2->setSpeed(high - 50);
      POS_M2->run(FORWARD); 
      NEG_M3->setSpeed(high - 50);
      NEG_M3->run(FORWARD); 
      NEG_M4->setSpeed(high - 50);
      NEG_M4->run(FORWARD);
      POS_M1->setSpeed(high - 50);
      POS_M1->run(FORWARD);
    }

    // Case XII: [0 0 1 1]
  if(readings[0] == 0 && readings[1] == 0 && readings[2] == 1 && readings[3] == 1){
        // slow turn right
        for (int i = low; i < high; i += 25) {
        POS_M2->setSpeed(i);
        POS_M2->run(FORWARD); 
        NEG_M3->setSpeed(i);
        NEG_M3->run(FORWARD); 
        // (If you want the inside wheels to not turn opposite way, uncomment this section)
        NEG_M4->setSpeed(low);
        NEG_M4->run(FORWARD);
        POS_M1->setSpeed(low);
        POS_M1->run(FORWARD); 
    }

    // Case XIII: [0 1 0 1]
  if (readings[0] == 0 && readings[1] == 1 && readings[2] == 0 && readings[3] == 1) {
      //sharper right
      POS_M2->setSpeed(high);
      POS_M2->run(FORWARD); 
      NEG_M3->setSpeed(high);
      NEG_M3->run(FORWARD); 
      //can toggle to 0 as well 
      NEG_M4->setSpeed(low);
      NEG_M4->run(FORWARD);
      POS_M1->setSpeed(low);
      POS_M1->run(FORWARD);
    }

    // Case XIV: [0 1 1 1]
  if (readings[0] == 0 && readings[1] == 1 && readings[2] == 1 && readings[3] == 1) {
      // Rotate CCW and move forward a little until left or right sensors detect black
      // Back wheel turns faster than front
      POS_M1->setSpeed(low);
      POS_M1->run(FORWARD); 
      NEG_M4->setSpeed(low);
      NEG_M4->run(FORWARD); 
      NEG_M3->setSpeed(high);
      NEG_M3->run(FORWARD);
      POS_M2->setSpeed(high);
      POS_M2->run(FORWARD);
    }

    // Case XV: [1 0 1 1]
  if (readings[0] == 1 && readings[1] == 0 && readings[2] == 1 && readings[3] == 1) {
      // move forward slowly?
      //toggle tmr
      POS_M1->setSpeed(mod);
      POS_M1->run(FORWARD); 
      NEG_M4->setSpeed(mod);
      NEG_M4->run(FORWARD); 
      NEG_M3->setSpeed(mod);
      NEG_M3->run(FORWARD);
      POS_M2->setSpeed(mod);
      POS_M2->run(FORWARD);
    }

    // Case XVI: [1 1 1 1]
  if (readings[0] == 1 && readings[1] == 1 && readings[2] == 1 && readings[3] == 1) {
      //go straight but slower
      POS_M2->setSpeed(0);
      POS_M2->run(FORWARD); 
      NEG_M3->setSpeed(0);
      NEG_M3->run(FORWARD); 
      NEG_M4->setSpeed(0);
      NEG_M4->run(FORWARD);
      POS_M1->setSpeed(0);
      POS_M1->run(FORWARD);
    }
  }
  }
}
