const int micPin = 2;       // Microphone sensor digital output pin
const int relayPin = 8;     // Relay control pin

bool relayState = false;    // false = OFF, true = ON
bool lastMicState = LOW;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200; // ms - debounce delay

void setup() {
  pinMode(micPin, INPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Initially OFF
  Serial.begin(9600);
}

void loop() {
  bool micState = digitalRead(micPin);

  if (micState == HIGH && lastMicState == LOW) {
    unsigned long currentTime = millis();
    if (currentTime - lastDebounceTime > debounceDelay) {
      // Toggle relay
      relayState = !relayState;
      digitalWrite(relayPin, relayState ? HIGH : LOW);
      Serial.print("Relay State: ");
      Serial.println(relayState ? "ON" : "OFF");
      lastDebounceTime = currentTime;
    }
  }

  lastMicState = micState;
}
