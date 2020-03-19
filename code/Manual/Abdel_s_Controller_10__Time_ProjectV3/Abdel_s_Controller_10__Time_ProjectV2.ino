#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// joystick
const bool inputX = 0;
const bool inputY = 1;

// buttons
const byte Up = 4;
unsigned long lastDebounceTimeUP = 0;
bool lastButtonStateUP = LOW;
bool buttonStateUP;

const byte Down = 3;
unsigned long lastDebounceTimeDOWN = 0;
bool lastButtonStateDOWN = LOW;
bool buttonStateDOWN;

const byte Stop = 2;
unsigned long lastDebounceTimeSTOP = 0;
bool lastButtonStateSTOP = LOW;
bool buttonStateSTOP;

struct DataPackage {
  byte XPos;
  byte YPos;
  bool ValUp;
  bool ValDown;
  bool StopProp;
};

struct DataPackage Data;

// radio pins and address
RF24 radio(7, 8); // defines the CE and CSN pins as pin 7 and 8
const byte address[6] = "00001";

void setup() {
  pinMode(Up, INPUT);
  pinMode(Down, INPUT);
  pinMode(Stop, INPUT);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();
}

void loop() {
  Data.XPos = map(analogRead(inputX), 0, 1023, 0, 255);
  Data.YPos = map(analogRead(inputY), 0, 1023, 0, 255);
  Data.ValUp = Debounce(Up , lastDebounceTimeUP , lastButtonStateUP , buttonStateUP);
  Data.ValDown = Debounce(Down , lastDebounceTimeDOWN , lastButtonStateDOWN , buttonStateDOWN);
  Data.StopProp = Debounce(Stop , lastDebounceTimeSTOP , lastButtonStateSTOP , buttonStateSTOP);

  radio.write(&Data, sizeof(DataPackage));
}


bool Debounce(int Button , unsigned long lastDebounceTime , int lastButtonState , bool buttonState) {


  unsigned long debounceDelay = 50;

  bool reading = digitalRead(Button);

  if (reading != lastButtonState) {

    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {

    if (reading != buttonState) {
      buttonState = reading;
    }
    return buttonState;

  }

  lastButtonState = reading;

}
