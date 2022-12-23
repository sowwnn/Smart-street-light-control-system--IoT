#include <ESP8266WiFi.h>   // Thư viện dùng để kết nối WiFi của ESP8266
#include <PubSubClient.h>  // Thư viện dùng để connect, publish/subscribe MQTT
#include <string.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>


// const char* ssid = "Huflit-GV";         // Tên của mạng WiFi mà bạn muốn kết nối đến
// const char* password = "gvhuflit@123";   // Mật khẩu của mạng WiFi

const char* ssid = "Sho0_";         // Tên của mạng WiFi mà bạn muốn kết nối đến
const char* password = "sownnnnn";  // Mật khẩu của mạng WiFi

// const char* ssid = "Lucid Coffee 2.4G";         // Tên của mạng WiFi mà bạn muốn kết nối đến
// const char* password = "lucidcoffee";  // Mật khẩu của mạng WiFi

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* mqttUser = "Sho";
const char* mqttPassword = "1234";


const int ledpin = D1;  // Đèn led ở chân GPIO2
const int motionpin = D7;
const String name = "3";

char messageBuff[100];  // Biến dùng để lưu nội dung tin nhắn

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);  // Khởi tạo kết nối Serial để truyền dữ liệu đến máy tính

  pinMode(ledpin, OUTPUT);
  pinMode(motionpin, INPUT);


  startWiFi();

  connectBroker();

  client.subscribe("sho/temp");
  client.publish("sho/temp", "Connected");
}

void loop() {
  if (!client.connected()) {
    connectBroker();
  }
  // motion = ;
  // Serial.println("Motion: "+  String(digitalRead(motionpin)));
  client.loop();
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
    if (client.connect("BlockyNodeClientID")) {  // Kêt nối đến broker thành công
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
  Serial.println(message);
  DynamicJsonDocument doc(256);
  deserializeJson(doc, message);

  if (message.indexOf("Mode") >= 0) {
    String leds = doc["Led"];
    int level = doc["Level"];

    if (leds.indexOf(name) >= 0){
        analogWrite(ledpin, level);
      }
    if (leds.indexOf("all") >= 0){
        analogWrite(ledpin, level);
      }
    }
  else {
    int led = doc["Led"];
    Serial.println("Led: " + String(led));
    Serial.println(" Motion: " + String(digitalRead(motionpin)));
    if (led == 50) {
      if (digitalRead(motionpin) == 1) {
        led = 255;
      } else {
        led = 50;
      }
      analogWrite(ledpin, led);
    } else {
      analogWrite(ledpin, led);
    }
  }
}