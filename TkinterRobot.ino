#include <Wire.h>
#include <NewPing.h>
#include <AccelStepper.h>
#include <SparkFun_MMA8452Q.h>
#include <Adafruit_MotorShield.h>

#define trigPin 12
#define echoPin 11
#define maxDistance 100
#define noEcho 0

MMA8452Q accel;

AccelStepper rStepper(AccelStepper::HALF4WIRE, 6, 7, 5, 4);
AccelStepper lStepper(AccelStepper::HALF4WIRE, 2, 3, 9, 8);
AccelStepper fStepper(AccelStepper::FULL4WIRE, A2, A3, A1, A0);

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *SL = AFMS.getMotor(1);
Adafruit_DCMotor *VC = AFMS.getMotor(2);
Adafruit_StepperMotor *bStepper = AFMS.getStepper(2048, 2);

NewPing dolphin (trigPin, echoPin, maxDistance);

byte resetNum;
byte runNum;
byte endNum;

unsigned long taskWait;
unsigned long currentMillis;

unsigned int pingSpeed = 60;
unsigned long pingTimer;
int minDistance = 5;

const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;
int dataNumber = 0;
char fR = 'f';
char aR = 'a';

int xAcceleration = accel.x;

int camPin = 13;

enum States {stateStart, stateIngest, stateRun};

States mState = stateStart;

bool accelInRange(int val, int minimum, int maximum) {
  return ((minimum <= val) && (val <= maximum));
}

boolean checkTime(unsigned long &lastMillis, unsigned long wait) {
  if (currentMillis - lastMillis >= wait) {
    lastMillis += wait;
    return true;
  }
  return false;
}

void baseRight() {
  bStepper->onestep(FORWARD, DOUBLE);
}
void baseLeft() {
  bStepper->onestep(BACKWARD, DOUBLE);
}

AccelStepper baseStepper(baseRight, baseLeft);

void serIngest()
{
  static byte ndx = 0;
  char endMarker = '>';
  char startMarker = 'b';
  char rc;

  if (Serial.available() > 0) {
    rc = Serial.read();
    if (rc == fR)
    {
      rStepper.setCurrentPosition(0);
      lStepper.setCurrentPosition(0);
      fStepper.setCurrentPosition(0);
      baseStepper.setCurrentPosition(0);
      int xAcceleration = accel.x;
      int front = 0;
      do
      {
        rStepper.moveTo(-1500);
        lStepper.moveTo(-1500);
        rStepper.run();
        lStepper.run();
        accel.read();
        int xAcceleration = accel.x;
        if (accelInRange(xAcceleration, -10, 10))
        {
          digitalWrite(A3, LOW);
          digitalWrite(A2, LOW);
          digitalWrite(A1, LOW);
          digitalWrite(A0, LOW);
        }
        else
        {
          if (xAcceleration > 0)
          {
            fStepper.moveTo(front++);
            fStepper.run();
          }
          if (xAcceleration < 0)
          {
            fStepper.moveTo(front--);
            fStepper.run();
          }
        }
      } while ((rStepper.distanceToGo() != 0) && ((accelInRange(xAcceleration, -10, 10) == true)));
      digitalWrite(A3, LOW);
      digitalWrite(A2, LOW);
      digitalWrite(A1, LOW);
      digitalWrite(A0, LOW);
      digitalWrite(6, LOW);
      digitalWrite(7, LOW);
      digitalWrite(5, LOW);
      digitalWrite(4, LOW);
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      digitalWrite(9, LOW);
      digitalWrite(8, LOW);
      if (digitalRead(6) == LOW)
      {
        do
        {
          baseStepper.moveTo(-720);
          baseStepper.run();
        } while (baseStepper.distanceToGo() != 0);
        bStepper->release();
        Serial.print("k");
      }
    }
    if (rc == aR)
    {
      rStepper.setCurrentPosition(0);
      lStepper.setCurrentPosition(0);
      fStepper.setCurrentPosition(0);
      baseStepper.setCurrentPosition(0);
      int xAcceleration = accel.x;
      int front = 0;
      do
      {
        rStepper.moveTo(-1500);
        lStepper.moveTo(-1500);
        rStepper.run();
        lStepper.run();
        accel.read();
        int xAcceleration = accel.x;
        if (accelInRange(xAcceleration, -10, 10))
        {
          digitalWrite(A3, LOW);
          digitalWrite(A2, LOW);
          digitalWrite(A1, LOW);
          digitalWrite(A0, LOW);
        }
        else
        {
          if (xAcceleration > 0)
          {
            fStepper.moveTo(front++);
            fStepper.run();
          }

          if (xAcceleration < 0)
          {
            fStepper.moveTo(front--);
            fStepper.run();
          }
        }
      } while ((rStepper.distanceToGo() != 0) && ((accelInRange(xAcceleration, -10 , 10) == true)));
      digitalWrite(A3, LOW);
      digitalWrite(A2, LOW);
      digitalWrite(A1, LOW);
      digitalWrite(A0, LOW);
      digitalWrite(6, LOW);
      digitalWrite(7, LOW);
      digitalWrite(5, LOW);
      digitalWrite(4, LOW);
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      digitalWrite(9, LOW);
      digitalWrite(8, LOW);
      Serial.println("k");
    }
    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      receivedChars[ndx] = '\0';
      ndx = 0;
      newData = true;
    }
    if (newData == true)
    {
      dataNumber = 0;
      dataNumber = atoi(receivedChars);
      if (dataNumber > 0)
      {
        runNum = 1;
        mState = stateRun;
      }
      newData = false;
    }
  }
}

void turn()
{
  rStepper.setCurrentPosition(0);
  lStepper.setCurrentPosition(0);
  fStepper.setCurrentPosition(0);
  baseStepper.setCurrentPosition(0);
  int xAcceleration = accel.x;
  int right = 0;
  int front = 0;
  int left = 0;
  do
  {
    baseStepper.moveTo(720);
    baseStepper.run();
  } while (baseStepper.distanceToGo() != 0);
  bStepper->release();
}

void armDown()
{
  rStepper.setCurrentPosition(0);
  lStepper.setCurrentPosition(0);
  fStepper.setCurrentPosition(0);
  baseStepper.setCurrentPosition(0);
  int xAcceleration = accel.x;
  int right = 0;
  int front = 0;
  int left = 0;

  while (dolphin.ping_cm() != minDistance)
  {
    pingTimer = millis();
    accel.read();
    int xAcceleration = accel.x;
    rStepper.moveTo(right++);
    lStepper.moveTo(left++);
    rStepper.run();
    lStepper.run();
    if (accelInRange(xAcceleration, -10, 10))
    {
      digitalWrite(A3, LOW);
      digitalWrite(A2, LOW);
      digitalWrite(A1, LOW);
      digitalWrite(A0, LOW);
    }
    else
    {
      if (xAcceleration > 0)
      {
        fStepper.moveTo(front++);
        fStepper.run();
      }
      if (xAcceleration < 0)
      {
        fStepper.moveTo(front--);
        fStepper.run();
      }
    }//else
  } //while
  NewPing::timer_stop();
  digitalWrite(A3, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A0, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(5, LOW);
  digitalWrite(4, LOW);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(9, LOW);
  digitalWrite(8, LOW);
}

void armUp()
{

  rStepper.setCurrentPosition(0);
  lStepper.setCurrentPosition(0);
  fStepper.setCurrentPosition(0);
  baseStepper.setCurrentPosition(0);
  int xAcceleration = accel.x;
  int front = 0; //front motor

  VC->run(FORWARD);
  {
    VC->setSpeed(150); //leaves vacuum on
  }
  // ********************************************
  //raises arm
  // ********************************************

  do
  {
    rStepper.moveTo(-1500);
    lStepper.moveTo(-1500);
    rStepper.run();
    lStepper.run();
    accel.read();
    int xAcceleration = accel.x;
    if (accelInRange(xAcceleration, -10, 10)) {
      digitalWrite(A3, LOW);
      digitalWrite(A2, LOW);
      digitalWrite(A1, LOW);
      digitalWrite(A0, LOW);
    }
    else {
      if (xAcceleration > 0) {
        fStepper.moveTo(front++);
        fStepper.run();
      }

      if (xAcceleration < 0) {
        fStepper.moveTo(front--);
        fStepper.run();
      }
    }
  } while ((rStepper.distanceToGo() != 0) && ((accelInRange(xAcceleration, -10, 10) == true)));
  digitalWrite(A3, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A0, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(5, LOW);
  digitalWrite(4, LOW);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(9, LOW);
  digitalWrite(8, LOW);
  //move base left

  if (digitalRead(6) == LOW)
  {
    do
    {
      baseStepper.moveTo(-720);
      baseStepper.run();
    }
    while (baseStepper.distanceToGo() != 0);
    bStepper->release();
  }
  //turn off vacuum
  VC->run(RELEASE);
}


void setup()
{
  Serial.begin(9600);
  AFMS.begin();
  accel.init(SCALE_4G, ODR_1);
  pinMode (A0, OUTPUT);
  pinMode (A1, OUTPUT);
  pinMode (A2, OUTPUT);
  pinMode (A3, OUTPUT);
  rStepper.setMaxSpeed (100);
  rStepper.setAcceleration(50);
  lStepper.setMaxSpeed (100);
  lStepper.setAcceleration(50);
  fStepper.setMaxSpeed (80);
  fStepper.setAcceleration(40);
  baseStepper.setMaxSpeed(80);
  baseStepper.setAcceleration(40);
  pinMode(camPin, OUTPUT);
  taskWait = millis();
  mState = stateStart;
}

void loop()
{
  currentMillis = millis();
  switch (mState)
  {
    case stateStart:
      {
        if (checkTime(taskWait, 1000UL))
        {
          taskWait = millis();
          mState = stateIngest;
        }
      }
      break; //stateStart
    case stateIngest:
      {
        serIngest();
      }
      break; //stateIngest
    case stateRun:
      {
        switch (runNum)
        {
          case 1:
            Serial.println(dataNumber);
            digitalWrite(camPin, HIGH);
            delay(4000);
            digitalWrite(camPin, LOW);
            delay(1000);
            runNum = 2;
            break; //case1
          case 2:
            turn();
            runNum = 3;
            break;//case2
          case 3:
            armDown();
            runNum = 4;
            break;//case3
          case 4:
            armUp();
            runNum = 5;
            break; //case4
          case 5:
            SL->run(FORWARD);
            {
              SL->setSpeed(250);
              delay(500);
            }
            SL->run(RELEASE);
            runNum = 6;
            break;//case5
          case 6:
            dataNumber--;
            if (dataNumber > 0)
            {
              runNum = 1;
              mState = stateRun;
            }
            if (dataNumber == 0)
            {
              Serial.println(dataNumber);
              mState = stateIngest;
            }
            taskWait = millis();
            break;//case
        }
        break;//switch(RunNum)
      }
      break;//stateRun
  }//mState
}//loop
