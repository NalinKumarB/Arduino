#include <WiFi.h>
#include <WebServer.h>

// Replace with your WiFi credentials
const char* ssid = "ROLEX";
const char* password = "NALIN1710";

WebServer server(80);

// GPIO pin
const int ledPin = 2;
bool pinState = false;

// HTML Webpage
String webpage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 WiFi Switch</title>
    <style>
        body { text-align: center; font-family: Arial; margin-top: 50px; }
        button {
            padding: 15px 30px;
            font-size: 20px;
            margin: 10px;
            border: none;
            border-radius: 10px;
            cursor: pointer;
        }
        .on { background-color: green; color: white; }
        .off { background-color: red; color: white; }
        .toggle { background-color: blue; color: white; }
    </style>
</head>
<body>
    <h1>ESP32 WiFi Switch</h1>
    <button class="on" onclick="fetch('/on')">ON</button>
    <button class="off" onclick="fetch('/off')">OFF</button>
    <button class="toggle" onclick="fetch('/toggle')">TOGGLE</button>
</body>
</html>
)rawliteral";

// Handlers
void handleRoot() {
    server.send(200, "text/html", webpage);
}

void handleOn() {
    digitalWrite(ledPin, HIGH);
    pinState = true;
    server.send(200, "text/plain", "ON");
}

void handleOff() {
    digitalWrite(ledPin, LOW);
    pinState = false;
    server.send(200, "text/plain", "OFF");
}

void handleToggle() {
    pinState = !pinState;
    digitalWrite(ledPin, pinState);
    server.send(200, "text/plain", "TOGGLED");
}

void setup() {
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);

    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConnected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    server.on("/on", handleOn);
    server.on("/off", handleOff);
    server.on("/toggle", handleToggle);

    server.begin();
}

void loop() {
    server.handleClient();
}