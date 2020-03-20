const int pingPin = 5;
const int echoPin = 6;

void setup() {
Serial.begin(9600);
}

void loop() {
 Serial.println(GetDistanceCM());
}

long GetDistanceCM(){
    pinMode(pingPin, OUTPUT);
    digitalWrite(pingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(pingPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(pingPin, LOW);
    pinMode(echoPin, INPUT);
    return pulseIn(echoPin,HIGH)/58;//was 29/2  
  }
