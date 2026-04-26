#include <WiFi.h>
#include <WebServer.h>
#include <time.h>

// Replace with your WiFi credentials
const char* ssid = "ROLEX";
const char* password = "NALIN1710";

// NTP Server for time synchronization
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 5.5 * 3600;           // IST (GMT+5:30)
const int daylightOffset_sec = 0;

WebServer server(80);

// GPIO pin
const int ledPin = 2;
bool pinState = false;

// Alarm variables
bool alarmEnabled = false;
int alarmHour = 7;
int alarmMinute = 0;
bool alarmTriggered = false;
unsigned long alarmStartTime = 0;
const unsigned long ALARM_DURATION = 2 * 60 * 1000; // 2 minutes in milliseconds

// HTML Webpage with Modern UI
String webpage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Smart Alarm Switch</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }
        
        .container {
            background: white;
            border-radius: 20px;
            box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);
            padding: 40px;
            max-width: 500px;
            width: 100%;
        }
        
        h1 {
            color: #333;
            text-align: center;
            margin-bottom: 30px;
            font-size: 28px;
        }
        
        .clock {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 20px;
            border-radius: 15px;
            text-align: center;
            margin-bottom: 30px;
            font-size: 24px;
            font-weight: bold;
        }
        
        .section {
            margin-bottom: 30px;
            padding: 20px;
            background: #f8f9fa;
            border-radius: 15px;
        }
        
        .section h2 {
            color: #667eea;
            font-size: 16px;
            margin-bottom: 15px;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        
        .button-group {
            display: flex;
            gap: 10px;
            flex-wrap: wrap;
        }
        
        button {
            flex: 1;
            min-width: 100px;
            padding: 12px 20px;
            font-size: 16px;
            border: none;
            border-radius: 10px;
            cursor: pointer;
            font-weight: bold;
            transition: all 0.3s ease;
        }
        
        .btn-on {
            background-color: #10b981;
            color: white;
        }
        
        .btn-on:hover {
            background-color: #059669;
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(16, 185, 129, 0.4);
        }
        
        .btn-off {
            background-color: #ef4444;
            color: white;
        }
        
        .btn-off:hover {
            background-color: #dc2626;
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(239, 68, 68, 0.4);
        }
        
        .btn-toggle {
            background-color: #3b82f6;
            color: white;
        }
        
        .btn-toggle:hover {
            background-color: #2563eb;
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(59, 130, 246, 0.4);
        }
        
        .btn-set {
            background-color: #667eea;
            color: white;
            flex: 1;
        }
        
        .btn-set:hover {
            background-color: #5568d3;
            transform: translateY(-2px);
        }
        
        .status {
            padding: 15px;
            border-radius: 10px;
            margin-bottom: 15px;
            font-weight: bold;
            text-align: center;
        }
        
        .status-on {
            background-color: #d1fae5;
            color: #065f46;
        }
        
        .status-off {
            background-color: #fee2e2;
            color: #7f1d1d;
        }
        
        .alarm-status {
            background-color: #fef3c7;
            color: #92400e;
        }
        
        .time-input {
            display: flex;
            gap: 10px;
            margin-bottom: 15px;
            justify-content: center;
        }
        
        input {
            width: 60px;
            padding: 10px;
            font-size: 16px;
            border: 2px solid #e5e7eb;
            border-radius: 8px;
            text-align: center;
            font-weight: bold;
        }
        
        input:focus {
            outline: none;
            border-color: #667eea;
        }
        
        .separator {
            font-size: 20px;
            font-weight: bold;
            color: #666;
        }
        
        .alarm-toggle {
            display: flex;
            align-items: center;
            gap: 10px;
            margin-bottom: 15px;
        }
        
        .switch {
            position: relative;
            display: inline-block;
            width: 50px;
            height: 24px;
        }
        
        .switch input {
            opacity: 0;
            width: 0;
            height: 0;
        }
        
        .slider {
            position: absolute;
            cursor: pointer;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background-color: #ccc;
            transition: 0.4s;
            border-radius: 24px;
        }
        
        .slider:before {
            position: absolute;
            content: "";
            height: 18px;
            width: 18px;
            left: 3px;
            bottom: 3px;
            background-color: white;
            transition: 0.4s;
            border-radius: 50%;
        }
        
        input:checked + .slider {
            background-color: #667eea;
        }
        
        input:checked + .slider:before {
            transform: translateX(26px);
        }
        
        .info {
            font-size: 12px;
            color: #6b7280;
            margin-top: 10px;
            text-align: center;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🔔 Smart Alarm Switch</h1>
        
        <div class="clock" id="clock">00:00:00</div>
        
        <!-- Manual Control Section -->
        <div class="section">
            <h2>Manual Control</h2>
            <div id="statusDisplay" class="status status-off">🔴 OFF</div>
            <div class="button-group">
                <button class="btn-on" onclick="controlPin('on')">ON</button>
                <button class="btn-off" onclick="controlPin('off')">OFF</button>
                <button class="btn-toggle" onclick="controlPin('toggle')">TOGGLE</button>
            </div>
        </div>
        
        <!-- Alarm Section -->
        <div class="section">
            <h2>Alarm Settings</h2>
            
            <div class="alarm-toggle">
                <label class="switch">
                    <input type="checkbox" id="alarmCheck" onchange="toggleAlarmUI()">
                    <span class="slider"></span>
                </label>
                <span>Enable Alarm</span>
            </div>
            
            <div id="alarmControl" style="display: none;">
                <div class="time-input">
                    <input type="number" id="hourInput" min="0" max="23" value="07" placeholder="HH">
                    <span class="separator">:</span>
                    <input type="number" id="minuteInput" min="0" max="59" value="00" placeholder="MM">
                </div>
                <button class="btn-set" onclick="setAlarm()">SET ALARM</button>
            </div>
            
            <div id="alarmStatusDisplay" style="display: none; margin-top: 15px;">
                <div class="status alarm-status" id="alarmStatusText">⏰ Alarm: 07:00</div>
            </div>
            
            <div class="info">⏱️ Alarm will activate for 2 minutes</div>
        </div>
    </div>

    <script>
        // Update clock every second
        function updateClock() {
            fetch('/gettime')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('clock').textContent = data.time;
                });
        }
        
        // Update status display
        function updateStatus() {
            fetch('/status')
                .then(response => response.json())
                .then(data => {
                    const statusDiv = document.getElementById('statusDisplay');
                    const alarmStatusDiv = document.getElementById('alarmStatusDisplay');
                    
                    if (data.pinState) {
                        statusDiv.textContent = '🟢 ON';
                        statusDiv.className = 'status status-on';
                    } else {
                        statusDiv.textContent = '🔴 OFF';
                        statusDiv.className = 'status status-off';
                    }
                    
                    if (data.alarmEnabled) {
                        alarmStatusDiv.style.display = 'block';
                        document.getElementById('alarmStatusText').textContent = 
                            '⏰ Alarm: ' + String(data.alarmHour).padStart(2, '0') + ':' + 
                            String(data.alarmMinute).padStart(2, '0');
                    } else {
                        alarmStatusDiv.style.display = 'none';
                    }
                });
        }
        
        function controlPin(action) {
            fetch('/' + action)
                .then(response => response.text())
                .then(data => updateStatus());
        }
        
        function toggleAlarmUI() {
            const checkbox = document.getElementById('alarmCheck');
            const alarmControl = document.getElementById('alarmControl');
            
            if (checkbox.checked) {
                alarmControl.style.display = 'block';
                fetch('/alarm/enable');
            } else {
                alarmControl.style.display = 'none';
                fetch('/alarm/disable');
            }
            updateStatus();
        }
        
        function setAlarm() {
            const hour = document.getElementById('hourInput').value;
            const minute = document.getElementById('minuteInput').value;
            
            fetch('/alarm/set?hour=' + hour + '&minute=' + minute)
                .then(response => response.text())
                .then(data => {
                    updateStatus();
                    alert('Alarm set for ' + String(hour).padStart(2, '0') + ':' + String(minute).padStart(2, '0'));
                });
        }
        
        // Initialize
        updateClock();
        updateStatus();
        setInterval(updateClock, 1000);
        setInterval(updateStatus, 2000);
    </script>
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
    alarmTriggered = false;
    server.send(200, "text/plain", "ON");
}

void handleOff() {
    digitalWrite(ledPin, LOW);
    pinState = false;
    alarmTriggered = false;
    server.send(200, "text/plain", "OFF");
}

void handleToggle() {
    pinState = !pinState;
    digitalWrite(ledPin, pinState);
    server.send(200, "text/plain", "TOGGLED");
}

void handleGetTime() {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", timeinfo);
    
    String json = "{\"time\":\"";
    json += timeStr;
    json += "\"}";
    
    server.send(200, "application/json", json);
}

void handleStatus() {
    String json = "{\"pinState\":";
    json += pinState ? "true" : "false";
    json += ",\"alarmEnabled\":";
    json += alarmEnabled ? "true" : "false";
    json += ",\"alarmHour\":";
    json += alarmHour;
    json += ",\"alarmMinute\":";
    json += alarmMinute;
    json += "}";
    
    server.send(200, "application/json", json);
}

void handleAlarmEnable() {
    alarmEnabled = true;
    server.send(200, "text/plain", "Alarm Enabled");
}

void handleAlarmDisable() {
    alarmEnabled = false;
    alarmTriggered = false;
    digitalWrite(ledPin, LOW);
    pinState = false;
    server.send(200, "text/plain", "Alarm Disabled");
}

void handleAlarmSet() {
    if (server.hasArg("hour") && server.hasArg("minute")) {
        alarmHour = server.arg("hour").toInt();
        alarmMinute = server.arg("minute").toInt();
        alarmTriggered = false;
        server.send(200, "text/plain", "Alarm Set");
    } else {
        server.send(400, "text/plain", "Missing parameters");
    }
}

void checkAlarm() {
    if (!alarmEnabled || alarmTriggered) return;
    
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    
    int currentHour = timeinfo->tm_hour;
    int currentMinute = timeinfo->tm_min;
    
    // Check if current time matches alarm time
    if (currentHour == alarmHour && currentMinute == alarmMinute && timeinfo->tm_sec < 5) {
        Serial.println("Alarm Triggered!");
        digitalWrite(ledPin, HIGH);
        pinState = true;
        alarmTriggered = true;
        alarmStartTime = millis();
    }
    
    // Turn off after 2 minutes if alarm was triggered
    if (alarmTriggered && (millis() - alarmStartTime) >= ALARM_DURATION) {
        Serial.println("Alarm Duration Complete - Turning Off");
        digitalWrite(ledPin, LOW);
        pinState = false;
        alarmTriggered = false;
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    
    Serial.println("\n\nStarting ESP32 WiFi Alarm Switch");
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✓ Connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        
        // Sync time with NTP
        Serial.println("Syncing time with NTP server...");
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        
        time_t now = time(nullptr);
        while (now < 24 * 3600) {
            delay(100);
            now = time(nullptr);
        }
        
        Serial.print("Current Time: ");
        Serial.println(ctime(&now));
    } else {
        Serial.println("\n✗ Failed to connect to WiFi");
    }
    
    // Server routes
    server.on("/", handleRoot);
    server.on("/on", handleOn);
    server.on("/off", handleOff);
    server.on("/toggle", handleToggle);
    server.on("/gettime", handleGetTime);
    server.on("/status", handleStatus);
    server.on("/alarm/enable", handleAlarmEnable);
    server.on("/alarm/disable", handleAlarmDisable);
    server.on("/alarm/set", handleAlarmSet);
    
    server.begin();
    Serial.println("Web Server started!");
}

void loop() {
    server.handleClient();
    checkAlarm();
    delay(100);
}
