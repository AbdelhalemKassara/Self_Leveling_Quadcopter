#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

struct Potentiometer {
  struct Joystick {
    static const byte X = 1;
    static const byte Y = 0;
  }; struct Joystick Joy;

  struct Throttle {
    static const byte Pin = 2;
    byte Val = 0;
  }; struct Throttle Throttle;

}; struct Potentiometer Pot;

struct Buttons {
  struct Stop {
    static const byte Pin = 2;
  }; struct Stop Stop;

}; struct Buttons But;

struct DataPackage {
  byte XPos;
  byte YPos;
  byte Throttle;
  bool StopProp;
};
struct DataPackage Data;

// radio pins and address
RF24 radio(7, 8); // defines the CE and CSN pins as pin 7 and 8
const byte address[6] = "00001";

void setup() {
  pinMode(But.Stop.Pin, INPUT);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();
}

void loop() {
  //joystick
  Data.XPos = map(analogRead(Pot.Joy.X), 0, 1023, 0, 255);
  Data.YPos = map(analogRead(Pot.Joy.Y), 0, 1023, 0, 255);

  //Throttle
  Pot.Throttle.Val = map(analogRead(Pot.Throttle.Pin), 0, 1023, 0, 255);//stores the value of the pot so that I won't need to run this twice in the next line
  Data.Throttle = Pot.Throttle.Val > 4 ? Pot.Throttle.Val : 0;// the potetiometer does not always fully go back to 0

  //Button
  Data.StopProp = digitalRead(But.Stop.Pin);

  radio.write(&Data, sizeof(DataPackage));
}
