#include<Wire.h>
#include <Servo.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h> 

// reciver
RF24 radio(7, 8); 
const byte address[6] = "00001";

// gyro sensor
const int MPU_addr = 0x68; // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

byte M1Speed; 
byte M2Speed; 
byte M3Speed; 
byte M4Speed; 

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
  //gyro/accel sensor
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  
  Wire.write(0);     
  Wire.endTransmission(true);

  // radio/ contorller
  radio.begin(); 
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX); 
  radio.startListening(); 
  
  
  // motors
  OutM1.attach(9, 1000, 2000);  
  OutM2.attach(3, 1000, 2000);  
  OutM3.attach(5, 1000, 2000);  
  OutM4.attach(6, 1000, 2000);  

  SetupMotorController(); 
}

void loop() {
  if (radio.available()) {  
    radio.read(&Data, sizeof(DataPackage)); 
  }
  
  GetPosition();



  // landing
  if (Data.StopProp == HIGH) { 
    AllMin(); 
  }

  UpdateSpeed();
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

void GetPosition() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 12, true); 
  //accelerometer
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  // gyroscope
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
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
