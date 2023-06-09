#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino_JSON.h>

// Data wire is plugged into port 15 on the ESP32
#define ONE_WIRE_BUS 15

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// variable to hold device addresses
DeviceAddress Thermometer;

int deviceCount = 0;

// Temp sensor addresses
uint8_t sensor1[8] = { 0x28, 0xE1, 0x0B, 0x94, 0x97, 0x13, 0x03, 0x96 };
uint8_t sensor2[8] = { 0x28, 0x65, 0x73, 0x80, 0xE3, 0xE1, 0x3D, 0x1D };
uint8_t sensor3[8] = { 0x28, 0x43, 0x16, 0x94, 0x97, 0x0A, 0x03, 0x6A };
uint8_t sensor4[8] = { 0x28, 0xCB, 0x30, 0x94, 0x97, 0x0A, 0x03, 0x45 };
float tempSensor1, tempSensor2, tempSensor3, tempSensor4;

// WiFi
const char* ssid = "<WIFI_SSID>";
const char* password = "<WIFI_PASSWORD>";

// MQTT Broker
const char* mqtt_server = "<MQTT_SERVER_ADDRESS>";

// LED Pin
const int ledPin = 4;

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[250];
int value = 0;

char payload[250];
char topic[250];
char buffer[250];

void printAddress(DeviceAddress deviceAddress)
{ 
  for (uint8_t i = 0; i < 8; i++)
  {
    Serial.print("0x");
    if (deviceAddress[i] < 0x10) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    if (i < 7) Serial.print(", ");
  }
  Serial.println("");
}

void readTemps(void) {
  sensors.requestTemperatures();
  tempSensor1 = sensors.getTempC(sensor1); // Gets the values of the temperature
  tempSensor2 = sensors.getTempC(sensor2); // Gets the values of the temperature
  tempSensor3 = sensors.getTempC(sensor3); // Gets the values of the temperature
  tempSensor4 = sensors.getTempC(sensor4); // Gets the values of the temperature
  Serial.println(tempSensor1);
  Serial.println(tempSensor2);
  Serial.println(tempSensor3);
  Serial.println(tempSensor4);
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup(void)
{
  // start serial port
  Serial.begin(115200);

  // Start up the library
  sensors.begin();

  // locate devices on the bus
  Serial.println("Locating devices...");
  Serial.print("Found ");
  deviceCount = sensors.getDeviceCount();
  Serial.print(deviceCount, DEC);
  Serial.println(" devices.");
  Serial.println("");
  
  Serial.println("Printing addresses...");
  for (int i = 0;  i < deviceCount;  i++)
  {
    Serial.print("Sensor ");
    Serial.print(i+1);
    Serial.print(" : ");
    sensors.getAddress(Thermometer, i);
    printAddress(Thermometer);
  }
  readTemps();
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    
    // request a read
    sensors.requestTemperatures();

    // read the temp sensors
    tempSensor1 = sensors.getTempC(sensor1);
    tempSensor2 = sensors.getTempC(sensor2);
    tempSensor3 = sensors.getTempC(sensor3);
    tempSensor4 = sensors.getTempC(sensor4);

    // Serial.println(payload);
    Serial.println(tempSensor1);

    String payload = "{";
    payload += "\"temp1_C\":";                         
    payload += tempSensor1;
    payload += ",";
    payload += "\"temp2_C\":";
    payload += tempSensor2;
    payload += ",";
    payload += "\"temp3_C\":";
    payload += tempSensor3;
    payload += ",";
    payload += "\"temp4_C\":";
    payload += tempSensor4;
    payload += ",";
    payload += "\"temp1_F\":";                         
    payload += (tempSensor1 * 1.8) + 32;
    payload += ",";
    payload += "\"temp2_F\":";
    payload += (tempSensor2 * 1.8) + 32;
    payload += ",";
    payload += "\"temp3_F\":";
    payload += (tempSensor3 * 1.8) + 32;
    payload += ",";
    payload += "\"temp4_F\":";
    payload += (tempSensor4 * 1.8) + 32;
    payload += "}";
    
    Serial.println(payload);
    char attributes[200];
    payload.toCharArray(attributes, 200);
    // payload.toCharArray(buffer, (payload.length() + 1));
    client.publish("esp32/temperature", attributes);

  }
}