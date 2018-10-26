#include <Arduino.h>
#include <ESPectro32_Board.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

// Your WiFi's SSID
const char* WIFI_SSID = "";

// Your WiFi's pasword
const char* WIFI_PASS = "";

// Your Flex IoT device's serial number or mac.
String DEVICE_SERIAL = "";


const char* MQTT_SERVER = "mqtt.flexiot.xl.co.id";
// Your Event Topic
const char* EVENT_TOPIC = "";

String SUB_TOPIC = "genericDevice/" + DEVICE_SERIAL + "/+/sub";

char msg[300];


WiFiClient wifiClient;
PubSubClient client(wifiClient);

int interval = 10*1000;
uint64_t timeSendMillis = 0;

// connect to WiFi
void setupWiFi() {
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    randomSeed(micros());

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void doActions(const char* message) {
    Serial.println(message);
}

// MQTT client callback
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(SUB_TOPIC);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    msg[i] = (char)payload[i];
  }

  doActions(msg);
}

// connect to MQTT broker.
void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESP32Client-";
        clientId += String(random(0xffff), HEX);

        Serial.print("client ID=");
        Serial.println(clientId);
        // Attempt to connect
        if (client.connect(clientId.c_str())) {
            Serial.println("connected");
            //subscribe to the topic
            // const char* SUB_TOPIC = SUB_TOPIC.c_str();
            client.subscribe(SUB_TOPIC.c_str());
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void publishMessage(const char* message){
  client.publish(EVENT_TOPIC,message);  
}

void sendPhotoTransistor() {
    float volt = ESPectro32.readPhotoTransistorVoltage();
    DynamicJsonBuffer buffer;
    JsonObject& root = buffer.createObject();
    root["mac"] = DEVICE_SERIAL;
    root["eventName"] = "phototransistor";
    root["phototransistor"] = volt;
    root["status"] = "none";

    Serial.printf("Volt: %f\n", volt);

    String payload;
    root.printTo(payload);

    Serial.println(payload);

    publishMessage(payload.c_str());
}

void setup() {
    Serial.begin(115200);
    setupWiFi();
    client.setServer(MQTT_SERVER, 1883);
    client.setCallback(callback);
}

void loop() {
    if(WiFi.status() != WL_CONNECTED){
        setupWiFi();  
    }

    if (WiFi.status()== WL_CONNECTED && !client.connected()) {
        reconnect();
    }

    // update timer and send data
    if (millis() - timeSendMillis >= interval) {
        timeSendMillis = millis();
        sendPhotoTransistor();
    }

    client.loop();
}


