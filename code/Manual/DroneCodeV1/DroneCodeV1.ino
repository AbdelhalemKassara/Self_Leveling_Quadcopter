#include <Servo.h>
#include <RF24.h>

RF24 radio(7, 8);
const byte address[6] = "00001";

boolean EStop = false;

// motors
// front
byte M1Speed; //black FL
byte M2Speed; //red FR
//rear
byte M3Speed; //yellow RL
byte M4Speed; //blue RR

float DistSens = 0.25; // the sensitivity of the distribution

Servo OutM1;
Servo OutM2;
Servo OutM3;
Servo OutM4;

struct DataPackage {
  byte XPos;
  byte YPos;
  byte Throttle;
  bool StopProp;
}; struct DataPackage Data;


void setup() {
  // motors
  OutM1.attach(9, 1000, 2000);
  OutM2.attach(3, 1000, 2000);
  OutM3.attach(6, 1000, 2000);
  OutM4.attach(5, 1000, 2000);

  SetupMotorController();

  // radio/ contorller
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();

}

void loop() {
  if (radio.available()) {  //if the controller is not conected
    radio.read(&Data, sizeof(DataPackage));

    if (EStop) {
      AllMin();
    }
    else {
      float DiffX = (Data.XPos - 128);//to bring the center value to 0 from 128
      float DiffY = (Data.YPos - 128);
      float AvSpeed = map(Data.Throttle, 0, 255, 0, 128 - DistSens * 128);
      float Sensitivity = DistSens * Data.Throttle / 510; // 2*255
//change the code so that it actually goes to the motor max speed which is 180 because in this code it is set to 128
      M1Speed = (byte)(AvSpeed + ((-DiffY + DiffX) * Sensitivity));//put motorspeed in an array
      M2Speed = (byte)(AvSpeed + ((-DiffY - DiffX) * Sensitivity));
      M3Speed = (byte)(AvSpeed + ((+DiffY + DiffX) * Sensitivity));
      M4Speed = (byte)(AvSpeed + ((+DiffY - DiffX) * Sensitivity));

      EStop = Data.StopProp;
    }
  }
  else { //if controller is not connected
    AllMin();
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
