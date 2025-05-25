#include <LiquidCrystal.h>

// LCD pin: RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(5, 6, 7, 8, 9, 10);

const int mq5Pin = 2;       // Digital output from MQ-5
const int buzzerPin = 3;    // Buzzer pin
const int tempStatusPin = 4; // HIGH if temperature > normal

void setup() {
  pinMode(mq5Pin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(tempStatusPin, INPUT);

  lcd.begin(16, 2);
  lcd.print("LPG Monitor");
  delay(2000);
  lcd.clear();
}

void loop() {
  int gasDetected = digitalRead(mq5Pin);
  int tempHigh = digitalRead(tempStatusPin);

  lcd.setCursor(0, 0);
  if (gasDetected == LOW) {
    lcd.print("LPG Detected!   ");
    digitalWrite(buzzerPin, HIGH);
  } else {
    lcd.print("Air: Clean      ");
    digitalWrite(buzzerPin, LOW);
  }

  lcd.setCursor(0, 1);
  if (tempHigh == HIGH) {
    lcd.print("Temp: HIGH      ");
    digitalWrite(buzzerPin, HIGH);
  } else {
    lcd.print("Temp: Normal    ");
    digitalWrite(buzzerPin, LOW);
  }

  delay(500);
}
