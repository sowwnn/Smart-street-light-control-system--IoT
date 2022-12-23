#include <ESP8266WiFi.h>   // Thư viện dùng để kết nối WiFi của ESP8266
#include <PubSubClient.h>  // Thư viện dùng để connect, publish/subscribe MQTT
#include <string.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>


// const char* ssid = "Huflit-GV";         // Tên của mạng WiFi mà bạn muốn kết nối đến
// const char* password = "gvhuflit@123";   // Mật khẩu của mạng WiFi

// const char* ssid = "Lucid Coffee 2.4G";         // Tên của mạng WiFi mà bạn muốn kết nối đến
// const char* password = "lucidcoffee";   // Mật khẩu của mạng WiFi

const char* ssid = "Sho0_";         // Tên của mạng WiFi mà bạn muốn kết nối đến
const char* password = "sownnnnn";  // Mật khẩu của mạng WiFi

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* mqttUser = "Sho";
const char* mqttPassword = "1234";
const String name = "1";

int led;
long water;
float t;
float h;
char buffer[256];
StaticJsonDocument<300> doc;
String mode = "auto";



SoftwareSerial serial_ESP(D2, D3);  //D2 = RX -- D3 = TX

#define ledpin D1
#define motionpin D7


char messageBuff[100];  // Biến dùng để lưu nội dung tin nhắn

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(ledpin, OUTPUT);
  pinMode(motionpin, INPUT);

  pinMode(D2, INPUT);
  pinMode(D3, OUTPUT);
  Serial.begin(9600);
  serial_ESP.begin(115200);

  startWiFi();
  connectBroker();

  client.subscribe("sho/temp");
  client.publish("sho/temp", "Connected");
}

void loop() {
  if (!client.connected()) {
    connectBroker();
  }
  client.loop();
  if (mode == "auto"){
    if (serial_ESP.available()) {
      DeserializationError err = deserializeJson(doc, serial_ESP);
      String dataRecive = serial_ESP.readString();

      serializeJson(doc, buffer);

      if (err == DeserializationError::Ok) {
        Serial.println("Led: " + String(doc["Led"].as<float>()) + " | Water: " + String(doc["Water"].as<float>()) + " |  Temp: " + String(doc["Temp"].as<float>()) + " |  Humd: " + String(doc["Humid"].as<float>()));
      }
      led = doc["Led"];
      int motion = digitalRead(motionpin);
      Serial.println("Motion: " + String(motion));
      if (led == 50) {
        if (motion == 1) {
          led = 255;
        }
      }
      client.publish("sho/temp", buffer);
    }
  }
}

void startWiFi() {
  WiFi.begin(ssid, password);  // Kết nối vào mạng WiFi
  Serial.print("Connecting to ");
  Serial.print(ssid);
  // Chờ kết nối WiFi được thiết lập
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n");
  Serial.println("Connection established!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  // Gởi địa chỉ IP đến máy tinh
}

void connectBroker() {
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ShoPucID")) {  // Kêt nối đến broker thành công
      Serial.println("\n");
      Serial.println("MQTT connected");
    } else {
      Serial.println("\n");
      Serial.print("MQTT failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  int i;
  for (i = 0; i < length; i++) {
    messageBuff[i] = (char)payload[i];
  }
  messageBuff[i] = '\0';
  String message = String(messageBuff);

  if (message.indexOf("control") >= 0) {
    deserializeJson(doc, message);
    mode = "control";
    String leds = doc["Led"];
    int level = doc["Level"];
    Serial.println("CONTROL");
    Serial.println(leds);
    Serial.println(String(level));


    if (leds.indexOf(name) >= 0) {
      analogWrite(ledpin, level);
    }
    if (leds.indexOf("all") >= 0) {
      analogWrite(ledpin, level);
    }
  } 
  else if (message.indexOf("auto") >= 0){
    mode = "auto";
  }
  if (mode=="auto"){
    analogWrite(ledpin, led);
  }
}