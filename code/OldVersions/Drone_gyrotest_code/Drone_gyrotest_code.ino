#include<Wire.h>
#include <Servo.h>
#include <SPI.h>

int GyX, GyY, GyZ;
long AcX, AcY, AcZ, TotalAcc;
int Tmp;
long GyXOff, GyYOff, GyZOff;
long Timer;
float GyPitch, GyRoll;
boolean set_gyro_angles;
float AccRoll, AccPitch;
float Pitch, Roll;
long height;
void setup() {
  Wire.begin();
  Serial.begin(57600);

  setup_mpu_6050_registers();
  OffSetGyro();

  Timer = micros();
}

void loop() {
  GetPosition();
  GyroAngles();
    Height();
  //Serial.println(AcZ);

//  Serial.println(GyRoll);
  // do movement here
  AccelAngles();

  CorrGyDrift();
  CompFilter();
    Serial.println(AccPitch);
  //Serial.println(Roll);

  while (micros() - Timer < 4000) {                               //Wait until the Timer reaches 4000us (250Hz) before starting the next loop
    //do nothing unitl the time reaches 4000us for the required 250Hz frequency
  }
  Timer = micros();

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
void Height(){
  height+= AcY;
  }

void AccelAngles() {

  TotalAcc = sqrt((AcX * AcX) + (AcY * AcY) + (AcZ * AcZ));
  //57.296 = 1 / (3.142 / 180)// The Arduino asin function is in radians
  AccPitch = asin((float)AcY / TotalAcc) * 57.296;
  AccRoll = asin((float)AcX / TotalAcc) * -57.296;

  //Place the MPU-6050 spirit level and note the values in the following two lines for calibration
  AccPitch -= -0.0;                                              //Accelerometer calibration value for pitch
  AccRoll -= -0.0;                                               //Accelerometer calibration value for roll


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
