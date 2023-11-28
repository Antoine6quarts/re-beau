#include <Arduino.h>

void setup() {
  Serial.begin(9600);
  delay(50);
}

void loop() {
  // format : 
  // x_axis_changs,y_axis_changes
  Serial.println("0,0");
  delay(3000);
  Serial.println("0,30");
  delay(3000);
  Serial.println("0,-30");
  delay(3000);
  Serial.println("15, 15");
  delay(3000);
  Serial.println("-15, -15");
  delay(3000);
  for (int i = 0; i < 360; ++i)
  {
    Serial.println("0, 1");
    delay(50);
  }
  delay(3000);
}