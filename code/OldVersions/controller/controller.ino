#include <SPI.h>  //library for spi
#include <nRF24L01.h> //library for 
#include <RF24.h>//library for 

// joystick
const int inputX = 0; // Left / Right
const int inputY = 1; //Forwards / Back

// buttons
const int Up = 4; //
const int Down = 3;
const int Stop = 2;

// using struct to get all the data into one pacage
struct DataPackage{
byte XPos;
byte YPos;
bool ValUp;
bool ValDown;
bool StopProp;
};

struct DataPackage Data;// Data is a variable that represents DataPackage

// radio pins and address
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";//

void setup() {
  Serial.begin(9600);
  pinMode(Up, INPUT);
  pinMode(Down, INPUT);
  pinMode(Stop, INPUT);
  
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
//reset

  
  // joystick x and y (forward, back, left and right) normal is 500 max and min 0 to around 1000
  Data.XPos = map(analogRead(inputX),0,1023,0,255); // bring the range from 0,1023 to 0,255
  Data.YPos = map(analogRead(inputY),0,1023,0,255);  // bring the range from 0,1023 to 0,255
  
  // throttle for going up and down
  Data.ValUp = digitalRead(Up);
  Data.ValDown = digitalRead(Down);
  
  // to stop the fans of the drone
  Data.StopProp = digitalRead(Stop);
  
  
  // test to see if controller works
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
  radio.write(&Data, sizeof(DataPackage));

}
