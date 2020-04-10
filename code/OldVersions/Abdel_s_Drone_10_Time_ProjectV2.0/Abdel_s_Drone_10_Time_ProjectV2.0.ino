#include<Wire.h>
#include <Servo.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8);
const byte address[6] = "00001";

// gyro sensor
int GyX, GyY, GyZ;
long AcX, AcY, AcZ, TotalAcc;// try variables with and int
int Tmp;
long GyXOff, GyYOff, GyZOff;
long Timer;
float GyPitch, GyRoll;
boolean set_gyro_angles;
float AccRoll, AccPitch;
float Pitch, Roll;

// motors
// front
//put in an array
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
  byte Throttle;
  bool StopProp;
};struct DataPackage Data;


void setup() {
  
  // motors
  OutM1.attach(9, 1000, 2000);
  OutM2.attach(3, 1000, 2000);
  OutM3.attach(5, 1000, 2000);
  OutM4.attach(6, 1000, 2000);

  SetupMotorController();

  //gyro/accel sensor
  Wire.begin();

  setup_mpu_6050_registers();
  OffSetGyro();

  // radio/ contorller
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();

  Timer = micros();
}

void loop() {
    if (radio.available()) {  // if angles are incorrect try 2 while loops using different timers
    radio.read(&Data, sizeof(DataPackage));
  }else{
  //write code for doing nothing when the contorller is not connected //waits for 5 seconds then tries to land the drone
  //when going down it waits until there is not acceleration in the y axis//create a function for landing 
  }

  // landing
  if (Data.StopProp == HIGH) {
    AllMin();
  }
  GetPosition();
  GyroAngles();
  //x get angles from here if you want to have very detailed
  // max motor speed is 180
  // need to get the height of the drone
  AccelAngles();
  CorrGyDrift();
  CompFilter();
  // get angles here if you want average or rounded


  UpdateSpeed();

  while (micros() - Timer < 4000) {                               //Wait until the Timer reaches 4000us (250Hz) before starting the next loop
    //do nothing unitl the time reaches 4000us for the required 250Hz frequency
  }
  Timer = micros();

}

void CalMotorSpeed(){
  
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

void GetPosition() {
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(0x68, 14);
  while (Wire.available() < 14);
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read();
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();
}

void OffSetGyro() {

  for (int i = 0; i < 2000 ; i ++) {
    GetPosition();
    GyXOff += GyX;
    GyYOff += GyY;
    GyZOff += GyZ;
    delay(3);                                                          //Delay 3us to simulate the 250Hz program loop
  }
  GyXOff /= 2000;
  GyYOff /= 2000;
  GyZOff /= 2000;

}

void setup_mpu_6050_registers() {
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();
  //Configure the accelerometer (+/-8g)
  Wire.beginTransmission(0x68);
  Wire.write(0x1C);
  Wire.write(0x10);
  Wire.endTransmission();
  //Configure the gyro (500dps full scale)
  Wire.beginTransmission(0x68);
  Wire.write(0x1B);
  Wire.write(0x08);
  Wire.endTransmission();
}

void GyroAngles() {
  GyX -= GyXOff;
  GyY -= GyYOff;
  GyZ -= GyZOff;

  //Gyro angle calculations
  //0.0000611 = 1 / (250Hz / 65.5)
  GyPitch += GyX * 0.0000611;
  GyRoll += GyY * 0.0000611;

  //0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians
  GyPitch += GyRoll * sin(GyZ * 0.000001066); //output values?? possibly for staying still
  GyRoll -= GyPitch * sin(GyZ * 0.000001066);// output values?? possibly for staying still
}

void AccelAngles() {
  TotalAcc = sqrt((AcX * AcX) + (AcY * AcY) + (AcZ * AcZ));
  //57.296 = 1 / (3.142 / 180)// The Arduino asin function is in radians
  AccPitch = asin((float)AcY / TotalAcc) * 57.296;
  AccRoll = asin((float)AcX / TotalAcc) * -57.296;

  //Place the MPU-6050 spirit level and note the values in the following two lines for calibration
  AccPitch -= -0.90;                                              //Accelerometer calibration value for pitch
  AccRoll -= 0.68;                                               //Accelerometer calibration value for roll
}

void CorrGyDrift() {
  if (set_gyro_angles) {
    GyPitch = GyPitch * 0.9996 + AccPitch * 0.0004;
    GyRoll = GyRoll * 0.9996 + AccRoll * 0.0004;
  }
  else {
    GyPitch = AccPitch;
    GyRoll = AccRoll;
    set_gyro_angles = true;
  }

}

void CompFilter() { // complementary filter
  //To dampen the pitch and roll angles a complementary filter is used
  Pitch = Pitch * 0.9 + GyPitch * 0.1; // output values?? possibly for movemnt
  Roll = Roll * 0.9 + GyRoll * 0.1; // output values??possibly for movemnt
}
