#define TRIG_PIN 4
#define ECHO_PIN 5

void setup() {
  Serial.begin(2000000);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  long startTime = millis();
  Serial.print(GetDistance());
  Serial.println(" cm");
  Serial.println(millis() - startTime);
}

float GetDistance() {
  digitalWrite(TRIG_PIN, LOW);          // This sets the trigger pin to low or 0v to make sure that the pin is not sending out a a signal
  delayMicroseconds(2);                 // This pauses the program for 2 microseconds
  digitalWrite(TRIG_PIN, HIGH);         // This sets the trigger pin to high or 5v to send out a signal
  digitalWrite(TRIG_PIN, LOW);          // This sets the trigger pin to low or 0v to finish the pulse
  float Time= 0;
 // Time= pulseIn (ECHO_PIN, HIGH);   // This returns the time starting from when the TrigPin went high to when the Echo pin goes high and makes it equal to Time
  Time /= 2000000; //converts the time from microseconds to seconds and divides the time by 2 (100000*2) because half dist

  float tempC;
  int tempReading;

  tempReading = analogRead(0);    // This gets the voltage back in the range of 0 to 1023 //the pin for the thermistor is 0
  tempC = log(10000.0 * ((1024.0 / tempReading - 1)));    //converts the voltage to Kelvin
  tempC = (1 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempC * tempC )) * tempC )) - 273.15;//converts voltage to kelvin then to celsius
  return ((331.5 + 0.6 * tempC) * Time) * 100;
}
