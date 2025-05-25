#define LDR_PIN A0
#define PIR_PIN 2
#define TRIG_PIN 3
#define ECHO_PIN 4
#define LED1 5
#define LED2 6
#define LED3 7

void setup() {
  Serial.begin(9600);

  pinMode(PIR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
}

void loop() {
  int ldrValue = analogRead(LDR_PIN);
  bool isDark = ldrValue < 100; // adjust threshold as needed
  bool motionDetected = digitalRead(PIR_PIN);

  // Measure distance with ultrasonic
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  int distance = duration * 0.034 / 2;

  Serial.print("LDR: ");
  Serial.print(ldrValue);
  Serial.print(" | Motion: ");
  Serial.print(motionDetected);
  Serial.print(" | Distance: ");
  Serial.println(distance);

  if (isDark) {
    if (motionDetected && distance < 100) { // object closer than 100cm
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);
      delay(5000);
    } else {
      digitalWrite(LED1, LOW); // or dim with PWM
      digitalWrite(LED2, LOW);
      digitalWrite(LED3, LOW);
    }
  } else {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
  }

  delay(200);
}

