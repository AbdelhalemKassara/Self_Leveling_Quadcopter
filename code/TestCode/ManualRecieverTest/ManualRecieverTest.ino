#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//radio
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

struct DataPackage {
  byte XPos;
  byte YPos;
  byte Throttle;
  bool StopProp;
};
struct DataPackage Data;

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

 Serial.write(12);//ASCII for a Form 
  Serial.println("Potentiometer");
  Serial.println(Data.XPos);
  Serial.println(Data.YPos);
  Serial.println("Throttle");
  Serial.println(Data.Throttle);
  Serial.println("Stop Button");-
  Serial.println(Data.StopProp);

  Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");// decreases the amount of flashing in putty
 }
}
