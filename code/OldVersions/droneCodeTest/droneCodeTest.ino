#include<Wire.h>
#include <Servo.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
// pins used 3,5,6,7,8,9,11,12
// pins that are not being used 0,1,2,4,13




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


void setup() {
  // motors
  OutM1.attach(9, 1000, 2000);  // (pin, min pulse width, max pulse width in microseconds)
  OutM2.attach(3, 1000, 2000);  //
  OutM3.attach(5, 1000, 2000);  //
  OutM4.attach(6, 1000, 2000);  //

  SetupMotorController(); //
}


// blue need wires to be flipped
//black needs to be flipped
//red is ok
// yellow is ok


void loop() {
   AllMax(); 
   
  UpdateSpeed();
delay(500);
exit(0);
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
