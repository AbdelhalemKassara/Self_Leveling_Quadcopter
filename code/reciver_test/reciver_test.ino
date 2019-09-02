/*
* Arduino Wireless Communication Tutorial
*       Example 1 - Receiver Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//radio
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";


// using struct to get all the data into one pacage

struct DataPackage1{
byte XPos;
byte YPos;
bool ValUp;
bool ValDown;
bool StopProp;
};

struct DataPackage1 Data1;// Data is a variable that represents DataPackage

struct DataPackage{
byte XPos;
byte YPos;
bool ValUp;
bool ValDown;
bool StopProp;
};

struct DataPackage Data;// Data is a variable that represents DataPackage

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
} 
void loop() {
  if (radio.available()) {
    
  radio.read(&Data, sizeof(DataPackage));
  
    if (Data1.XPos !=Data.XPos||
Data1.YPos != Data.YPos||
Data1.ValUp!= Data.ValUp||
Data1.ValDown!= Data.ValDown||
Data1.StopProp!= Data.StopProp){
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
    Data1.XPos =Data.XPos;
Data1.YPos = Data.YPos;
Data1.ValUp= Data.ValUp;
Data1.ValDown= Data.ValDown;
Data1.StopProp= Data.StopProp;
    }
  }
}
