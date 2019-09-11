#include<Wire.h>
#include <Servo.h>
#include <SPI.h>
// look at variables and compare with mpu compare code

// gyro sensor
const int MPU_addr = 0x68; // I2C address of the MPU-6050
int AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
long GyXOff, GyYOff, GyZOff;
float GyRoll, GyPitch, GyYaw;
float AccRoll, AccPitch, AccYaw;//
float Roll, Pitch, Yaw;// final output
bool set_gyro_angles;
long Timer;
long TotalAcc;

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
 
//  YawTransfer();
  // do movement here
  //AccelAngles();
 
// CorrGyDrift();
 // CompFilter();
  while(micros() - Timer < 4000){                                 //Wait until the Timer reaches 4000us (250Hz) before starting the next loop
  //do nothing unitl the time reaches 4000us for the required 250Hz frequency
  }
  Timer = micros();  

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
  for (int i = 1; i <= 500; i++) {
    GetPosition();
    GyZOff += GyZ;
    GyYOff += GyY;
    GyXOff += GyX;
    delay(3);
  }
  GyZOff /= 500;
  GyYOff /= 500;
  GyXOff /= 500;

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
  Serial.println(GyX);
  Serial.println(GyPitch);

  Timer = micros();

}

void YawTransfer(){
  //0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians
  GyPitch += GyRoll * sin(GyZ * 0.000001066);               //If the IMU has yawed transfer the roll angle to the pitch angel
  GyRoll -= GyPitch * sin(GyZ * 0.000001066);                //If the IMU has yawed transfer the pitch angle to the roll angel
  }
  
void AccelAngles(){
   TotalAcc = sqrt((AcX*AcX)+(AcY*AcY)+(AcZ*AcZ));  //Calculate the total accelerometer vector
  //57.296 = 1 / (3.142 / 180) The Arduino asin function is in radians
  AccPitch = asin((float)AcY/TotalAcc)* 57.296;       //Calculate the pitch angle
  AccRoll = asin((float)AcX/TotalAcc)* -57.296;       //Calculate the roll angle
  
    //Place the MPU-6050 spirit level and note the values in the following two lines for calibration
  AccPitch -= 0.0;                                              //Accelerometer calibration value for pitch
  AccRoll -= 0.0;                                               //Accelerometer calibration value for roll

  }

void CorrGyDrift(){
      if(set_gyro_angles){                                                 //If the IMU is already started
    GyPitch = GyPitch * 0.9996 + AccPitch * 0.0004;     //Correct the drift of the gyro pitch angle with the accelerometer pitch angle
    GyRoll = GyRoll * 0.9996 + AccRoll * 0.0004;        //Correct the drift of the gyro roll angle with the accelerometer roll angle
  }
  else{                                                                //At first start
    GyPitch = AccPitch;                                     //Set the gyro pitch angle equal to the accelerometer pitch angle 
    GyRoll = AccRoll;                                       //Set the gyro roll angle equal to the accelerometer roll angle 
    set_gyro_angles = true;                                            //Set the IMU started flag
  }
}

void CompFilter(){ // complementary filter
  Pitch = Pitch * 0.9 + GyPitch * 0.1;   //Take 90% of the output pitch value and add 10% of the raw pitch value
  Roll = Roll * 0.9 + GyRoll * 0.1;      //Take 90% of the output roll value and add 10% of the raw roll value
  }
