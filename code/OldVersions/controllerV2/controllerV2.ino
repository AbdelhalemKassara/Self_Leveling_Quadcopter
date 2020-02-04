#include <SPI.h>  //library for spi
#include <nRF24L01.h> //library for 
#include <RF24.h>//library for 

// joystick
const int inputX = 0; // Left / Right
const int inputY = 1; //Forwards / Back

// buttons
const int Up = 4; 
// variabls for debounce for up
unsigned long lastDebounceTimeUP = 0;
bool lastButtonStateUP = LOW;
bool buttonStateUP;

const int Down = 3;
// variabls for debounce for down
unsigned long lastDebounceTimeDOWN = 0;
bool lastButtonStateDOWN = LOW;
bool buttonStateDOWN;

const int Stop = 2;
// variabls for debounce for stop
unsigned long lastDebounceTimeSTOP = 0;
bool lastButtonStateSTOP = LOW;
bool buttonStateSTOP;


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
  
  // joystick x and y (forward, back, left and right) normal is 500 max and min 0 to around 1000
  Data.XPos = map(analogRead(inputX),0,1023,0,255); // bring the range from 0,1023 to 0,255
  Data.YPos = map(analogRead(inputY),0,1023,0,255);  // bring the range from 0,1023 to 0,255
  
  // throttle for going up and down
  Data.ValUp = Debounce(Up ,lastDebounceTimeUP ,lastButtonStateUP ,buttonStateUP);
  Data.ValDown = Debounce(Down ,lastDebounceTimeDOWN ,lastButtonStateDOWN ,buttonStateDOWN);
  
  // to stop the fans of the drone
  Data.StopProp = Debounce(Stop ,lastDebounceTimeSTOP ,lastButtonStateSTOP ,buttonStateSTOP);
  
  radio.write(&Data, sizeof(DataPackage));

}


bool Debounce(int Button ,unsigned long lastDebounceTime ,int lastButtonState ,bool buttonState){//

// the following variables are unsigned longs because the time, measured in milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers



  // read the state of the switch into a local variable:
  bool reading = digitalRead(Button);

  
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis(); // records the last time from when the arduino started in meleseconds 
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {

    if (reading != buttonState) { // it sees if the button is equal to what the last button state was eg buttonState equal HIGH for it to go into the if statement reading would need to be LOW
      buttonState = reading; // this makes buttonState equal to reading
      }
      return buttonState;// this makes the function equal to ButtonState
    
  }

  lastButtonState = reading;// this makes lastButtonState equal to reading

}
