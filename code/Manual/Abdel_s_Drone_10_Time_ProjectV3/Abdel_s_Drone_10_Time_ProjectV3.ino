#include <Servo.h>
#include <RF24.h>

RF24 radio(7, 8);
const byte address[6] = "00001";

// motors
// front
float M1Speed; //black FL
float M2Speed; //red FR
//rear
float M3Speed; //yellow RL
float M4Speed; //blue RR

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
  Serial.begin(9600);
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
  if (radio.available()) {  //if the controller and drone are conected
    radio.read(&Data, sizeof(DataPackage));

    float testX = (Data.XPos - 128);
    float testY = (Data.YPos - 128);//* DistSens
    M1Speed =  map(Data.Throttle, 0, 255, 0, 128 - DistSens * 128) + ((-testY + testX) / 255) * DistSens * (Data.Throttle / 2); //goes past the max when throttle is set to max
    M2Speed =  map(Data.Throttle, 0, 255, 0, 128 - DistSens * 128) + ((-testY - testX) / 255) * DistSens * (Data.Throttle / 2); // (Data.Throttle/2)
    M3Speed =  map(Data.Throttle, 0, 255, 0, 128 - DistSens * 128) + ((+testY + testX) / 255) * DistSens * (Data.Throttle / 2); //max 1 min -1
    M4Speed =  map(Data.Throttle, 0, 255, 0, 128 - DistSens * 128) + ((+ testY - testX) / 255) * DistSens * (Data.Throttle / 2); //sens * max = subtract

    Serial.write(12);//ASCII for a Form
    Serial.println(M1Speed);
    Serial.println(M2Speed);
    Serial.println(M3Speed);
    Serial.println(M4Speed);
    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");// decreases the amount of flashing in putty

  }
  else { //if they are not connected

  }


  //UpdateSpeed();//updates the speed of the propellers of the drone once every loop
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
