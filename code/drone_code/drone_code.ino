#include<Wire.h>
#include <Servo.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
// pins used 3,5,6,7,8,9,11,12
// pins that are not being used 0,1,2,4,13

// reciver
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

// gyro sensor
const int MPU_addr = 0x68; // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;


// speed of each motor
byte M1Speed;
byte M2Speed;
byte M3Speed;
byte M4Speed;

// motors
Servo OutM1;
Servo OutM2;
Servo OutM3;
Servo OutM4;

// datapackage coming from the controller
struct DataPackage{
byte XPos;
byte YPos;
bool ValUp;
bool ValDown;
bool StopProp;
};

struct DataPackage Data;// Data is a variable that represents DataPackage


void setup() {
  //gyro/accel sensor
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

// radio/ contorller  
radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  
  // motors
  OutM1.attach(9, 1000, 2000);  // (pin, min pulse width, max pulse width in microseconds)
  OutM2.attach(3, 1000, 2000);  //
  OutM3.attach(5, 1000, 2000);  //
  OutM4.attach(6, 1000, 2000);  //

  SetupMotorController(); //
}

void loop() {
    
    // controller/ radio
    if (radio.available()) {
  radio.read(&Data, sizeof(DataPackage));
 
  // test to see if the data is coming from the controller
  Serial.print("Stop button: ");
  Serial.println(Data.StopProp);

Serial.print("UP button: ");
  Serial.println(Data.ValUp);

  Serial.print("Down button");
  Serial.println(Data.ValDown);
Serial.println();
  Serial.print("Joystick x: ");
  Serial.println(Data.XPos);

  Serial.print("Joystick y: ");
  Serial.println(Data.YPos);
 
    }


    
  GetPosition();// get the xyz of gyro and accelrometer

  //  M#Speed scale it to use it with the servo library (value between 0 and 180)

  // landing
  if (Data.StopProp == HIGH) {
    AllMin();
  }

  //add normal flight
  // if up = high then increase the elevation

  // if

  UpdateSpeed();
}


// functions
void UpdateSpeed() {
  OutM1.write(M1Speed);    // Send the signal to the ESC (value needs to be between 0 to 180)
  OutM2.write(M2Speed);    // Send the signal to the ESC
  OutM3.write(M3Speed);    // Send the signal to the ESC
  OutM4.write(M4Speed);    // Send the signal to the ESC
}

void SetupMotorController() {
  delay(1000);// whait for the motor controller to get setup

    AllMin();// set all of the speeds of the motor to 0
    delay(2000);// waits for the motor controller to 2 second

  AllMax(); // sets all of the speeds of the motor to 180
  delay(2000);  // waits for the motor controller 2 seconds

  AllMin(); // sets all of speeds of the motor to 0
  delay(2000);// waits for the motor controller for 2 seconds 

}

void GetPosition() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true); // request a total of 12 registers
  //accelerometer
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  // gyroscope
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  //magnometer???
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

  void KalmanFilter(){
    
    
    }
  
