#include <WiFi.h>
#include "PubSubClient.h"
const char* ssid = "SAWALA Office";
const char* password = "sawala28";
const char* mqttServer = "mqtt.ardumeka.com";
int port = 11219;
String stMac;
char mac[50];
char clientId[50];
WiFiClient espClient;
PubSubClient client(espClient);
const int ledPin = 2;

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
// Uncomment according to your hardware type
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
//#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW
#define MAX_DEVICES 4
#define CS_PIN 5
MD_Parola Display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

#define relay1 4
#define relay2 16

int jumlah_objek;

void setup() {
  Serial.begin(115200);

  pinMode(relay1, OUTPUT);
  digitalWrite(relay1, HIGH);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay2, HIGH);

  Display.begin();
  Display.setIntensity(0);
  Display.displayClear();

  Display.setTextAlignment(PA_CENTER);
  Display.print("...");

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  wifiConnect();
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
  stMac = WiFi.macAddress();
  stMac.replace(":", "_");
  Serial.println(stMac);
  client.setServer(mqttServer, port);
  client.setCallback(callback);
  pinMode(ledPin, OUTPUT);
}

void wifiConnect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void mqttReconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    long r = random(1000);
    sprintf(clientId, "clientId-%ld", r);
    if (client.connect(clientId)) {
      Serial.print(clientId);
      Serial.println(" connected");
      client.subscribe("kelasrobot/h721312hg1/data");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {

  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");

  String stMessage;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    stMessage += (char)message[i];
  }
  Serial.println();
  Serial.println("Jumlah Objek: " + stMessage);

  Display.setTextAlignment(PA_CENTER);
  Display.print(stMessage);

  jumlah_objek = stMessage.toInt();
  if (jumlah_objek == 0) {
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
  }
  if ((jumlah_objek > 0) && (jumlah_objek <= 5)) {
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, HIGH);
  }
  if (jumlah_objek > 5) {
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, LOW);
  }
}

void loop() {
  delay(10);
  if (!client.connected()) {
    mqttReconnect();
  }
  client.loop();
}
