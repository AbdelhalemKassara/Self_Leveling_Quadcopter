#include<Wire.h>
#include <Servo.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// gyro sensor
const int MPU_addr = 0x68; // I2C address of the MPU-6050
int AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
long GyXOff, GyYOff, GyZOff;
int GyRoll, GyPitch, GyYaw;
int AccRoll, AccPitch, AccYaw;//
float Roll, Pitch, Yaw;// final output

long Timer;
long TotalAcc;

void setup() {
  Wire.begin();
  Serial.begin(9600);

  setup_mpu_6050_registers();
  OffSetGyro();

  Timer = micros();
}

void loop() {
  GetPosition();
  GyroAngles();
  YawTransfer();

  while (micros() - CurrentTime < 4000); {

  }                                //Wait until the loop_timer reaches 4000us (250Hz) before starting the next loop

}
void PrintPositionData() {

  Serial.print("AcX = "); Serial.print(AcX);
  Serial.print(" | AcY = "); Serial.print(AcY);
  Serial.print(" | AcZ = "); Serial.print(AcZ);
  Serial.print(" | Tmp = "); Serial.print(Tmp / 340.00 + 36.53); //equation for temperature in degrees C from datasheet
  Serial.print(" | GyX = "); Serial.print(GyX);
  Serial.print(" | GyY = "); Serial.print(GyY);
  Serial.print(" | GyZ = "); Serial.println(GyZ);
}

void GetPosition() {
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x3B);                                                    //Send the requested starting register
  Wire.endTransmission();                                              //End the transmission
  Wire.requestFrom(0x68, 14);                                          //Request 14 bytes from the MPU-6050
  while (Wire.available() < 14);                                       //Wait until all the bytes are received
  AcX = Wire.read() << 8 | Wire.read();                              //Add the low and high byte to the acc_x variable
  AcY = Wire.read() << 8 | Wire.read();                             //Add the low and high byte to the acc_y variable
  AcZ = Wire.read() << 8 | Wire.read();                             //Add the low and high byte to the acc_z variable
  Tmp = Wire.read() << 8 | Wire.read();                        //Add the low and high byte to the temperature variable
  GyX = Wire.read() << 8 | Wire.read();                             //Add the low and high byte to the gyro_x variable
  GyY = Wire.read() << 8 | Wire.read();                             //Add the low and high byte to the gyro_y variable
  GyZ = Wire.read() << 8 | Wire.read();                             //Add the low and high byte to the gyro_z variable
}

void OffSetGyro() {
  for (int i = 1; i <= 2000; i++) {
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

void setup_mpu_6050_registers() {
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

void GyroAngles() {
  GyZ -= GyZOff;
  GyY -= GyYOff;
  GyX -= GyXOff;

  //0.0000611 = 1 / (250Hz / 65.5)
  GyPitch += GyX * 0.0000611;
  GyRoll += GyY * 0.0000611;
  GyYaw += GyZ * 0.0000611;

  Timer = micros();

}

void YawTransfer(){
  //0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians
  GyPitch += GyRoll * sin(GyYaw * 0.000001066);               //If the IMU has yawed transfer the roll angle to the pitch angel
  GyRoll -= GyPitch * sin(GyYaw * 0.000001066);               //If the IMU has yawed transfer the pitch angle to the roll angel
  //need to fix
  }
  
void AccelAngles(){
    TotalAcc = sqrt((AcX*AcX)+(AcY*AcY)+(AcZ*AcZ));  //Calculate the total accelerometer vector
   
   //57.296 = 1 / (3.142 / 180) The Arduino asin function is in radians
  angle_pitch_acc = asin((float)acc_y/acc_total_vector)* 57.296;       //Calculate the pitch angle
  angle_roll_acc = asin((float)acc_x/acc_total_vector)* -57.296;       //Calculate the roll angle
  
  
  }
