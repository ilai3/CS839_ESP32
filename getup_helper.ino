/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-door-status-monitor-email/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <WiFi.h>

// Set GPIOs for LED and reedswitch
const int sensorPin = 14;
const int led = 13; //optional
const int button = 0;

// Detects whenever the door changed state
bool changeState = false;
bool changeButtonState = false;

// Holds reedswitch state (1=opened, 0=close)
bool Sensorstate;
String doorState;
bool buttonState = false;

// Auxiliary variables (it will only detect changes that are 1500 milliseconds apart)
unsigned long previousMillis = 0; 
const long interval = 1500;

const char* ssid = "Grass iPhone";
const char* password = "";
const char* host = "maker.ifttt.com";
const char* apiKey = "bspdsOIp2jjUnwIyShFUqi";

// Runs whenever the reedswitch changes state
ICACHE_RAM_ATTR void changeSensorStatus() {
  if (Sensorstate == false){
    changeState = true;
  }
}

ICACHE_RAM_ATTR void changeButtonStatusEvent() {
//  if (buttonState == false){
//    Serial.println("Button State changed");
    changeButtonState = true;
//  }
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Read the current door state
  pinMode(sensorPin, INPUT);
  Sensorstate = digitalRead(sensorPin);

  // Set LED state to match door state
  pinMode(led, OUTPUT);
  digitalWrite(led, Sensorstate);
  
  // Set the reedswitch pin as interrupt, assign interrupt function and set CHANGE mode
  attachInterrupt(digitalPinToInterrupt(sensorPin), changeSensorStatus, CHANGE);
  attachInterrupt(digitalPinToInterrupt(button), changeButtonStatusEvent, CHANGE);
  Serial.println("WiFi started");
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");  
}

void loop() {
  if (changeState){  
      Sensorstate = !Sensorstate;
      if(Sensorstate) {
        doorState = "closed";
      }
      else{
        doorState = "open";
      }
      digitalWrite(led, Sensorstate);
      changeState = false;
      Sensorstate = false;
      //Send email
      Serial.print("connecting to ");
      Serial.println(host);
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      }

      String url = "/trigger/news/json/with/key/cdMGnKaKscME3ACdrbJQgo";

      Serial.print("Requesting URL: ");
      Serial.println(url);
      client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" + 
                     "Content-Type: application/x-www-form-urlencoded\r\n" + 
                     "Content-Length: 13\r\n\r\n" +
                     "value1=" + doorState + "\r\n");
       delay(30000);
       client.stop();
  }
  if (changeButtonState){
      digitalWrite(led, false);
      Serial.print("connecting to ");
      Serial.println(host);
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      }

      String url = "/trigger/Sensor_status/json/with/key/bspdsOIp2jjUnwIyShFUqi";

      Serial.print("Requesting URL: ");
      Serial.println(url);
      client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" + 
                     "Content-Type: application/json\r\n" + 
                     "Content-Length: 13\r\n\r\n"+
                     "value1=" + "xx" + "\r\n");

      changeButtonState = false;
      delay(10000);
      client.stop();
  }
}
