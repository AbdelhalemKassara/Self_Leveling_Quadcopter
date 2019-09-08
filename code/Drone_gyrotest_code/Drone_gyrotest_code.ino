#include<Wire.h>
#include <Servo.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h> 

// gyro sensor
const int MPU_addr = 0x68; // I2C address of the MPU-6050
int AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
   long GyXOff;
   long GyYOff;
   long GyZOff;
    long DeltaT;
    int Roll;
    int Pitch;
    int Yaw;
     long CurrentTime;
     long LastTime;
void setup() {
  
  //gyro/accel sensor
  Wire.begin();
setup_mpu_6050_registers();
    Serial.begin(9600);
OffSetGyro();
}

void loop() {
  GetPosition();
    GyroAngles();
  Serial.println(Roll);
  
  while(micros() - CurrentTime < 4000); {
    
    }                                //Wait until the loop_timer reaches 4000us (250Hz) before starting the next loop
  
 
}
void PrintPositionData(){
  
  Serial.print("AcX = "); Serial.print(AcX);
  Serial.print(" | AcY = "); Serial.print(AcY);
  Serial.print(" | AcZ = "); Serial.print(AcZ);
  Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
  Serial.print(" | GyX = "); Serial.print(GyX);
  Serial.print(" | GyY = "); Serial.print(GyY);
     Serial.print(" | GyZ = "); Serial.println(GyZ);
  }


void GetPosition() {
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x3B);                                                    //Send the requested starting register
  Wire.endTransmission();                                              //End the transmission
  Wire.requestFrom(0x68,14);                                           //Request 14 bytes from the MPU-6050
  while(Wire.available() < 14);                                        //Wait until all the bytes are received
  AcX = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_x variable
  AcY= Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_y variable
  AcZ= Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_z variable
  Tmp = Wire.read()<<8|Wire.read();                            //Add the low and high byte to the temperature variable
  GyX = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the gyro_x variable
  GyY = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the gyro_y variable
  GyZ = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the gyro_z variable  
}
void OffSetGyro(){
  for (int i=1;i <=2000; i++){
  GetPosition();
  GyZOff += GyZ;
  GyYOff += GyY;
  GyXOff += GyX;
  delay(3);
  }
  GyZOff /= 2000;
  GyYOff /= 2000;
  GyXOff /= 2000;

  }

  void setup_mpu_6050_registers(){
  //Activate the MPU-6050
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x6B);                                                    //Send the requested starting register
  Wire.write(0x00);                                                    //Set the requested starting register
  Wire.endTransmission();                                              //End the transmission
  //Configure the accelerometer (+/-8g)
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x1C);                                                    //Send the requested starting register
  Wire.write(0x10);                                                    //Set the requested starting register
  Wire.endTransmission();                                              //End the transmission
  //Configure the gyro (500dps full scale)
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x1B);                                                    //Send the requested starting register
  Wire.write(0x08);                                                    //Set the requested starting register
  Wire.endTransmission();                                              //End the transmission
}
void GyroAngles(){
    GyZ -= GyZOff;
    GyY -= GyYOff;
    GyX -= GyXOff;

    CurrentTime = micros();
  //  Serial.println(CurrentTime);
     DeltaT = (CurrentTime - LastTime);
    Roll += GyX*(0.0000611); //(DeltaT/1000000);
   
    LastTime = CurrentTime; 
  }
