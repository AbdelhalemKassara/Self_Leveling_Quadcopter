#include <Servo.h>
#include <RF24.h>

RF24 radio(7, 8);
const byte address[6] = "00001";

// motors
// front
byte M1Speed; //black FL
byte M2Speed; //red FR
//rear
byte M3Speed; //yellow RL
byte M4Speed; //blue RR

Servo OutM1;
Servo OutM2;
Servo OutM3;
Servo OutM4;

struct DataPackage {
  byte XPos;
  byte YPos;
  bool ValUp;
  bool ValDown;
  bool StopProp;
};

struct DataPackage Data;


void setup() {
  // motors
  OutM1.attach(9, 1000, 2000);
  OutM2.attach(3, 1000, 2000);
  OutM3.attach(5, 1000, 2000);
  OutM4.attach(6, 1000, 2000);

  SetupMotorController();

  // radio/ contorller
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();

}

void loop() {
  
  if (radio.available()) {  //gets data from the controller
    radio.read(&Data, sizeof(DataPackage));
  }


  UpdateSpeed();//updates the speed of the propellers of the drone once every loop
}


// functions
void UpdateSpeed() {
  OutM1.write(M1Speed);
  OutM2.write(M2Speed);
  OutM3.write(M3Speed);
  OutM4.write(M4Speed);
}

void SetupMotorController() {
  delay(1000);

  AllMin();
  delay(2000);

  AllMax();
  delay(2000);

  AllMin();
  delay(2000);
}

void AllMax() {
  M1Speed = 180;
  M2Speed = 180;
  M3Speed = 180;
  M4Speed = 180;
  UpdateSpeed();
}

void AllMin() {
  M1Speed = 0;
  M2Speed = 0;
  M3Speed = 0;
  M4Speed = 0;
  UpdateSpeed();

}
