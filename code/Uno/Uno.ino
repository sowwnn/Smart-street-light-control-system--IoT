#include "DHT.h"
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#define DHTPIN 2


const int ledPin = 3;
const int lightsen = A0;

#define DHTTYPE DHT11  // DHT 11
SoftwareSerial espSerial(5, 6);
DHT dht(DHTPIN, DHTTYPE);


#define sensorPower 7
#define watersen A1

String str;

void setup() {
  Serial.begin(9600);
  espSerial.begin(115200);
  dht.begin();

  pinMode(ledPin, OUTPUT);
  pinMode(lightsen, INPUT);



  pinMode(sensorPower, OUTPUT);
  // digitalWrite(sensorPower, HIGH);
}
void loop() {
  StaticJsonDocument<200> doc;
  float h, t = humtemp();
  // serializeJson(doc, espSerial);

  delay(2000);

  int sensorValue = analogRead(lightsen);
  float voltage = sensorValue * (5.0 / 1023.0);
  Serial.println(voltage);
  int led = 0;
  int water = 0;
  if (voltage < 0.3) 
  {
    water = readSensor();
    if (water > 250) {
      led = 255;
    } else {
      led = 50;
    }
  } 
  else {
   led = 0;
  }
  Serial.println("Led: " + String(led) + " | Water: " + String(water) + " |  Temp: " + String(t) + " |  Humd: " + String(h));
  analogWrite(ledPin, led);
  doc["Led"] = led;
  doc["Water"] = water;
  doc["Temp"] = t;
  doc["Humid"] = h;
  serializeJson(doc, espSerial);
  delay(500);
}

int readSensor() {
  delay(10);                         // wait 10 milliseconds
  float val = analogRead(watersen);  // Read the analog value form sensor
  return val;                        // send current reading
}

int humtemp() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)) {
    return;
  }
  return h, f;
}