#include <Servo.h>
int thing=0;
// pins used 3,5,6,7,8,9,11,12
// pins that are not being used 0,1,2,4,13




// speed of each motor
int M1Speed;

// motors
Servo OutM1;     


void setup() {
 //0-180
 //180-0
 //0-20

  OutM1.attach(9, 1000, 2000); // (pin, min pulse width, max pulse width in microseconds)

 
// setup motors 


  delay(1000);// 6 seconds
  
  M1Speed = 0;// max speed
 
OutM1.write(M1Speed);    // Send the signal to the ESC (value needs to be between 0 to 180)

  delay(2000);// 1 second
  M1Speed = 180; // min speed
 
  OutM1.write(M1Speed);    // Send the signal to the ESC (value needs to be between 0 to 180)
delay(2000);

 M1Speed = 0; // min speed
 
  OutM1.write(M1Speed);    // Send the signal to the ESC (value needs to be between 0 to 180)
delay(2000);

}

void loop() {
 if (thing ==0){
  //  M#Speed scale it to use it with the servo library (value between 0 and 180)
  M1Speed = 180; // min speed

OutM1.write(M1Speed);    // Send the signal to the ESC (value needs to be between 0 to 180)
 thing++; 
delay(7000);
 }
   M1Speed = 0; // min speed


OutM1.write(M1Speed);    // Send the signal to the ESC (value needs to be between 0 to 180)
 // exit(0);
  
    }
