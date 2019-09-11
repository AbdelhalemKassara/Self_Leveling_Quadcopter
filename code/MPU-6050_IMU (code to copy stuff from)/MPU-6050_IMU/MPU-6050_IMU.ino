#include <Wire.h>

//Declaring some global variables
int GyX, GyY, GyZ; // changed
long AcX, AcY, AcZ, TotalAcc;// changed
int Tmp;// changed
long GyXOff, GyYOff, GyZOff;// changed 
long Timer;// changed 
float GyPitch, GyRoll; // changed
boolean set_gyro_angles;
float AccRoll, AccPitch;// changed
float Pitch, Roll;// changed 



void setup() {
  Wire.begin();                                                        //Start I2C as master
  Serial.begin(57600);                                               //Use only for debugging
  
  setup_mpu_6050_registers();                                          //Setup the registers of the MPU-6050 (500dfs / +/-8g) and start the gyro

                                                //Set the LCD cursor to position to position 0,1
  for (int i = 0; i < 2000 ; i ++){                  //Run this code 2000 times
    read_mpu_6050_data();                                              //Read the raw acc and gyro data from the MPU-6050
    GyXOff += GyX;                                              //Add the gyro x-axis offset to the GyXOff variable
    GyYOff += GyY;                                              //Add the gyro y-axis offset to the GyYOff variable
    GyZOff += GyZ;                                              //Add the gyro z-axis offset to the GyZOff variable
    delay(3);                                                          //Delay 3us to simulate the 250Hz program loop
  }
  GyXOff /= 2000;                                                  //Divide the GyXOff variable by 2000 to get the avarage offset
  GyYOff /= 2000;                                                  //Divide the GyYOff variable by 2000 to get the avarage offset
  GyZOff /= 2000;                                                  //Divide the GyZOff variable by 2000 to get the avarage offset

  

  
  Timer = micros();                                               //Reset the loop timer
}

void loop(){
 

//  read_mpu_6050_data();                                                //Read the raw acc and gyro data from the MPU-6050/

  //GyX -= GyXOff;                                                //Subtract the offset calibration value from the raw GyX value
  //GyY -= GyYOff;                                                //Subtract the offset calibration value from the raw GyY value
 // GyZ -= GyZOff;                                                //Subtract the offset calibration value from the raw GyZ value
  
  //Gyro angle calculations
  //0.0000611 = 1 / (250Hz / 65.5)
 // GyPitch += GyX * 0.0000611;                                   //Calculate the traveled pitch angle and add this to the angle_pitch variable
 // GyRoll += GyY * 0.0000611;                                    //Calculate the traveled roll angle and add this to the angle_roll variable
  
  //0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians
 // GyPitch += GyRoll * sin(GyZ * 0.000001066);               //If the IMU has yawed transfer the roll angle to the pitch angel
 // GyRoll -= GyPitch * sin(GyZ * 0.000001066);               //If the IMU has yawed transfer the pitch angle to the roll angel
  
  //Accelerometer angle calculations
 // TotalAcc = sqrt((AcX*AcX)+(AcY*AcY)+(AcZ*AcZ));  //Calculate the total accelerometer vector
  //57.296 = 1 / (3.142 / 180) The Arduino asin function is in radians
 // AccPitch = asin((float)AcY/TotalAcc)* 57.296;       //Calculate the pitch angle/
//  AccRoll = asin((float)AcX/TotalAcc)* -57.296;       //Calculate the roll angle
  
  //Place the MPU-6050 spirit level and note the values in the following two lines for calibration
//  AccPitch -= 0.0;                                              //Accelerometer calibration value for pitch
//  AccRoll -= 0.0;                                               //Accelerometer calibration value for roll
//
  if(set_gyro_angles){                                                 //If the IMU is already started
    GyPitch = GyPitch * 0.9996 + AccPitch * 0.0004;     //Correct the drift of the gyro pitch angle with the accelerometer pitch angle
    GyRoll = GyRoll * 0.9996 + AccRoll * 0.0004;        //Correct the drift of the gyro roll angle with the accelerometer roll angle
  }
  else{                                                                //At first start
    GyPitch = AccPitch;                                     //Set the gyro pitch angle equal to the accelerometer pitch angle 
    GyRoll = AccRoll;                                       //Set the gyro roll angle equal to the accelerometer roll angle 
    set_gyro_angles = true;                                            //Set the IMU started flag
  }
  
  //To dampen the pitch and roll angles a complementary filter is used
  Pitch = Pitch * 0.9 + GyPitch * 0.1;   //Take 90% of the output pitch value and add 10% of the raw pitch value
  Roll = Roll * 0.9 + GyRoll * 0.1;      //Take 90% of the output roll value and add 10% of the raw roll value

  while(micros() - Timer < 4000);                                 //Wait until the Timer reaches 4000us (250Hz) before starting the next loop
  Timer = micros();  
//Reset the loop timer
}


void read_mpu_6050_data(){                                             //Subroutine for reading the raw gyro and accelerometer data
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x3B);                                                    //Send the requested starting register
  Wire.endTransmission();                                              //End the transmission
  Wire.requestFrom(0x68,14);                                           //Request 14 bytes from the MPU-6050
  while(Wire.available() < 14);                                        //Wait until all the bytes are received
  AcX = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the AcX variable
  AcY = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the AcY variable
  AcZ = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the AcZ variable
  Tmp = Wire.read()<<8|Wire.read();                            //Add the low and high byte to the Tmp variable
  GyX = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the GyX variable
  GyY = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the GyY variable
  GyZ = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the GyZ variable

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
