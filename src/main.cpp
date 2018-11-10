#include <Arduino.h>
#include <ESPectro32_Board.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

// Your WiFi's SSID
// TOOD: edit the variable's value below
const char *WIFI_SSID = "";

// Your WiFi's pasword
// TOOD: edit the variable's value below
const char *WIFI_PASS = "";

// Your Flex IoT device's serial number or mac.
// TOOD: edit the variable's value below
String DEVICE_SERIAL = "";

// MQTT server configuration
const char *MQTT_SERVER = "mqtt.flexiot.xl.co.id";
const char *MQTT_USER = "rabbit";
const char *MQTT_PASS = "rabbit";
// TODO: Fill the MQTT_ID value with your unique device name!
const char *MQTT_ID = "YOUR_DEVICE_ID";

// FlexIoT Topics
// TODO: edit the values of these variables, these values are just an example!
const char *EVENT_TOPIC = "generic_brand_617generic_device3/common";
String ACTION_TOPIC = "+/" + DEVICE_SERIAL + "/generic_brand_617/generic_device/3/sub";
String ACTION_RESPONSE_TOPIC = "/generic_brand_617/generic_device/3/pub";

char msg[300];

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// connect to WiFi
void setupWifi()
{
    // We start by connecting to a WiFi network
    delay(100);
    Serial.print("\nConnecting to");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(100);
        Serial.print("-");
    }
    Serial.print("\nConnected to");
    Serial.println(WIFI_SSID);
}

// MQTT client callback
void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Received messages; ");
    Serial.println(topic);

    int topicLen = strlen(topic);
    String correlationId = "";

    for (int i = 0; i < topicLen; i++)
    {
        if (topic[i] == '/')
        {
            break;
        }

        correlationId += topic[i];
    }

    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }

    // TOOD: fill the action response value!
    // e.g: String response = "{\"led\":\"1\"}";

    String response = "";

    // or easier with ArduinoJson as shown below
    //
    // DynamicJsonBuffer jsonBuffer;
    // JsonObject& root = jsonBuffer.createObject();
    // root["led"] = "1";
    // root.printTo(response);

    String responseTopic = correlationId + ACTION_RESPONSE_TOPIC;

    Serial.print("Publishing: ");
    Serial.println(response);
    Serial.print("Topic: ");
    Serial.println(responseTopic);
    client.publish(responseTopic.c_str(), response.c_str());

    Serial.println();
}

// connect to MQTT broker.
void reconnect()
{
    while (!client.connected())
    {
        Serial.print("\nConnecting to");
        Serial.println(MQTT_SERVER);

        if (client.connect(MQTT_ID, MQTT_USER, MQTT_PASS))
        {
            Serial.print("\nConnected to");
            Serial.println(MQTT_SERVER);
            client.subscribe(ACTION_TOPIC.c_str());
        }
        else
        {
            Serial.print("\nTrying connect again");
            delay(5000);
        }
    }
}

void setup()
{
    Serial.begin(115200);
    setupWifi();
    client.setServer(MQTT_SERVER, 1883);
    client.setCallback(callback);
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }

    // TOOD: Fill your event message value!
    // e.g: String message = "{\"eventName\":\"phototransistor\",\"status\":\"none\",\"phototransistor\":0.1395,\"mac\":\"\"}"";
    String message = "";

    // or use ArduinoJson as shown below
    //
    // DynamicJsonBuffer jsonBuffer;
    // JsonObject& root = jsonBuffer.createObject();
    // root["eventName"] = "phototransistor";
    // root["status"] = "none";
    // root["phototransistor"] = 0.1;
    // root["mac"] = DEVICE_SERIAL;
    // root.printTo(message);

    Serial.print("Sending messages: ");
    Serial.println(message);
    client.publish(EVENT_TOPIC, message.c_str());
    client.loop();
    delay(5000);
}
