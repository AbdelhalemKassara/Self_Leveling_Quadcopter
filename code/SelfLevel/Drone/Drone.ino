#include<Wire.h>
#include <Servo.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8);
const byte address[6] = "00001";
long TimeSinceLastCon = 0;

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
const int MaxTurnAngle = 30;//stores the max value that the drone can change is angle by

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
}; struct DataPackage Data;

bool EStop = false;

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

  //timer for the 250hz frequency of the program
  Timer = micros();
}

void loop() {
  if (radio.available()) {
    radio.read(&Data, sizeof(DataPackage));
    TimeSinceLastCon = micros();//stores the last time the controllers was connected

  } else {
    SetNutralValForJoyStick();
    if (micros() - TimeSinceLastCon > 5000000) { //if the controller has not been connected in 5 seconds
      //when going down it waits until there is not acceleration in the y axis//create a function for landing
      //call landing function here
      Landing();
    }
  }

  GetPosition();
  GyroAngles();
  Leveling();
  AccelAngles();
  CorrGyDrift();
  CompFilter();
  UpdateSpeed();

  while (micros() - Timer < 4000) {                               //Wait until the Timer reaches 4000us (250Hz) before starting the next loop
    //do nothing unitl the time reaches 4000us for the required 250Hz frequency
  }
  Timer = micros();
}

// functions
void Landing() {
  GetPosition();
  if (AcY > -100) {
    if (Data.Throttle = 0) {
      exit(0);
    } else {
      Data.Throttle--;
    }
  }
  GyroAngles();
  Leveling();
  AccelAngles();
  CorrGyDrift();
  CompFilter();
  UpdateSpeed();

}

void Leveling() {
  if ((abs(GyPitch) > MaxTurnAngle) || (abs(GyRoll) > MaxTurnAngle)) {
    M1Speed = 0;
    M2Speed = 0;
    M3Speed = 0;
    M4Speed = 0;
  }
  else {
    float DistSens = 0.25;//to control the sensitivity of the drone

    float Pitch = (Data.XPos - 128) + (GyPitch / MaxTurnAngle) * 128;
    float Roll  = (Data.YPos - 128) + (GyRoll / MaxTurnAngle) * 128; // adds the speed needed for turning and leveling the drone
    float AvSpeed = map(Data.Throttle, 0, 255, 0, 180 - DistSens * 360);//180 * 2 for the controller and self leveling(180 is the max speed of the motors)
    float Sensitivity = DistSens * Data.Throttle / 510; // 2*255

    M1Speed = (byte)(AvSpeed + ((-Roll + Pitch) * Sensitivity));//put motorspeed in an array
    M2Speed = (byte)(AvSpeed + ((-Roll - Pitch) * Sensitivity));
    M3Speed = (byte)(AvSpeed + ((+Roll + Pitch) * Sensitivity));
    M4Speed = (byte)(AvSpeed + ((+Roll - Pitch) * Sensitivity));
  }
}


void SetNutralValForJoyStick() {
  Data.XPos = 128;
  Data.YPos = 128;
}

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
