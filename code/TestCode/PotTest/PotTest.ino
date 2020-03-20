int val = 0;

void setup() {
Serial.begin(9600);
}

void loop() {
val = analogRead(2);// pot pin is pin 2
Serial.println(val);//prints out the value
}
