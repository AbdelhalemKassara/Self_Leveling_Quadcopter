#include <EEPROM.h>

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.update(3, 255); // writes 0 only if the value in cell 3 is not 0 (does not write the same value multiple times)

  for (int i = 0; i < EEPROM.length(); i++) {
    Serial.println(EEPROM[i]);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
