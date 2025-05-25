#include <SoftwareSerial.h>

SoftwareSerial sim800(7,8); //RX, TX for SIM800L

void setup() {
  Serial.begin(9600);
  sim800.begin(9600);
  delay(1000);
  sendCommand("AT");
  sendCommand("AT+CMGF=1");
  sendCommand("AT+CSCS=\"GSM\"");
  sendSMS("+918610169125","Hello Mr.BNK \n This message is from SIM800");
}

void loop() {
  
}

void sendSMS(String number, String message){
  sim800.println("AT+CMGS=\""+number+"\"");
  delay(100);
  sim800.println(message);
  delay(100);
  sim800.write(26);
  delay(3000);
}

void sendCommand(String cmd){
  sim800.println(cmd);
  delay(500);
  while(sim800.available()){
    Serial.write(sim800.read());
  }
}