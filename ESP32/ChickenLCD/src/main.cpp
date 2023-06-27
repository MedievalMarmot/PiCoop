#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

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
char msg[350];
int value = 0;

char payload[512];
char topic[250];
char buffer[512];
char temp1buff[10];
char temp2buff[10];
char temp3buff[10];
char temp4buff[10];

char tempFbuff[10];
char humidityBuff[10];
char dewPointBuff[10];
char airPressBuff[10];

// make some custom characters:
byte Heart[8] = {
0b00000,
0b01010,
0b11111,
0b11111,
0b01110,
0b00100,
0b00000,
0b00000
};

byte Bell[8] = {
0b00100,
0b01110,
0b01110,
0b01110,
0b11111,
0b00000,
0b00100,
0b00000
};


byte Alien[8] = {
0b11111,
0b10101,
0b11111,
0b11111,
0b01110,
0b01010,
0b11011,
0b00000
};

byte Check[8] = {
0b00000,
0b00001,
0b00011,
0b10110,
0b11100,
0b01000,
0b00000,
0b00000
};

byte Speaker[8] = {
0b00001,
0b00011,
0b01111,
0b01111,
0b01111,
0b00011,
0b00001,
0b00000
};


byte Sound[8] = {
0b00001,
0b00011,
0b00101,
0b01001,
0b01001,
0b01011,
0b11011,
0b11000
};


byte Skull[8] = {
0b00000,
0b01110,
0b10101,
0b11011,
0b01110,
0b01110,
0b00000,
0b00000
};

byte Lock[8] = {
0b01110,
0b10001,
0b10001,
0b11111,
0b11011,
0b11011,
0b11111,
0b00000
};

// Set LCD address
LiquidCrystal_I2C lcd(0x27,20,4); 

void callback(char* topic, byte* message, unsigned int length) {
  StaticJsonDocument<512> topicMessage;
  String messageTemp;
  String coopStatus;
  
  // custom status icon to display
  int status;

  // average of all 4 probes in the coops
  int avgCoopTemp = 0;

  // set up vars
  double temp1 = 0;
  double temp2 = 0;
  double temp3 = 0;
  double temp4 = 0;
  
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // grab data from topic and format / output
  if (String(topic) == "esp32/temperature") {
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(topicMessage, messageTemp);

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    
    temp1 = topicMessage["temp1_F"];
    temp2 = topicMessage["temp2_F"];
    temp3 = topicMessage["temp3_F"];
    temp4 = topicMessage["temp4_F"];

    dtostrf(temp1, 1, 2, temp1buff);
    dtostrf(temp2, 1, 2, temp2buff);
    dtostrf(temp3, 1, 2, temp3buff);
    dtostrf(temp4, 1, 2, temp4buff);

    // avg coop temps
    avgCoopTemp = ((temp1 + temp2 + temp3 + temp4) / 4);

    // create LCD status outputs based on temp
    if ((avgCoopTemp <= 0) || (avgCoopTemp > 120)) {
      coopStatus = "No data";
      status = 1;
    }

    if ((avgCoopTemp > 94) && (avgCoopTemp< 106 )) {
      coopStatus = "Very Hot!";
      status = 1;
    }

    if ((avgCoopTemp > 84) && (avgCoopTemp< 96 )) {
      coopStatus = "Hot!";
      status = 1;      
    }

    if ((avgCoopTemp > 74) && (avgCoopTemp< 86 )) {
      coopStatus = "Warm";
      status = 1;
    }

    if ((avgCoopTemp > 64) && (avgCoopTemp< 76 )) {
      coopStatus = "Perfect! :)";
      status = 0;
    }

    if ((avgCoopTemp > 54) && (avgCoopTemp< 66 )) {
      coopStatus = "Cool";
      status = 1;
    }

    if ((avgCoopTemp > 44) && (avgCoopTemp< 56 )) {
      coopStatus = "Cold!";
      status = 1;
    }

    if ((avgCoopTemp > 34) && (avgCoopTemp< 46 )) {
      coopStatus = "Very Cold!";
      status = 1;
    }

    // Display outputs to LCD
    String line0 = "T1: ";
    line0 += temp1buff;
    line0 += " T2: ";
    line0 += temp2buff;
    
    String line1 = "B1: ";
    line1 += temp3buff;
    line1 += " B2: ";
    line1 += temp4buff;

    lcd.setCursor ( 0, 0 );
    lcd.print("                    ");
    lcd.setCursor ( 0, 1 );
    lcd.print("                    ");

    lcd.setCursor ( 0, 0 );
    lcd.print(line0);
    lcd.setCursor ( 0, 1 );
    lcd.print(line1);

    delay(3000);
   
    lcd.setCursor ( 0, 0 );
    lcd.print("                    ");
    lcd.setCursor ( 0, 1 );
    lcd.print("                    ");

    lcd.setCursor ( 0, 0 );
    lcd.write(status);
    lcd.setCursor ( 2, 0 );
    lcd.print(coopStatus);

    lcd.setCursor ( 0, 1 );
    lcd.print("Average Temp:");
    lcd.setCursor ( 14, 1 );
    lcd.print(avgCoopTemp);

    delay(3000);
  }

  // grab data from topic and format / output
  if (String(topic) == "rPi/ambientEnv") {
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(topicMessage, messageTemp);

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    double tempF = topicMessage["temperature_F"];
    double humidityP = topicMessage["humidity"];
    double dewPoint = topicMessage["dewPoint_C"];
    double airPressure = topicMessage["pressure_Hg"];
  
    dewPoint = (dewPoint * 9/5);

    dtostrf(tempF, 1, 2, tempFbuff);
    dtostrf(humidityP, 1, 2, humidityBuff);
    dtostrf(dewPoint, 1, 2, dewPointBuff);
    dtostrf(airPressure, 1, 2, airPressBuff);

    String line2 = "AT: ";
    line2 += tempFbuff;
    line2 += " H%: ";
    line2 += humidityBuff;
    
    String line3 = "DP: ";
    line3 += dewPointBuff;
    line3 += " AP: ";
    line3 += airPressBuff;

    lcd.setCursor ( 0, 2 );
    lcd.print("                    ");
    lcd.setCursor ( 0, 3 );
    lcd.print("                    ");

    lcd.setCursor ( 0, 2 );
    lcd.print(line2);
    lcd.setCursor ( 0, 3 );
    lcd.print(line3);

    delay(3000);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266ClientLCD")) {
      Serial.println("connected");
      // Subscribe
      // client.subscribe("esp32/temperature");
      client.subscribe("esp32/temperature");
      client.subscribe("rPi/ambientEnv");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  // start serial port
  Serial.begin(115200);
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
  client.setBufferSize(512);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32ClientLCD")) {
      Serial.println("connected");  
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
 
  // Subscribe to the MQTT topics for environment to display on LCD
  client.subscribe("esp32/temperature");
  client.subscribe("rPi/ambientEnv");

  lcd.init();  //initialize the lcd
  lcd.backlight();  //open the backlight

  // create a new characters
  lcd.createChar(0, Heart);
  lcd.createChar(1, Bell);
  lcd.createChar(2, Alien);
  lcd.createChar(3, Check);
  lcd.createChar(4, Speaker);
  lcd.createChar(5, Sound);
  lcd.createChar(6, Skull);
  lcd.createChar(7, Lock);
  
  lcd.setCursor ( 0, 0 );            // go to the top left corner
  lcd.print("Welcome to piCoop!  "); // write this string on the top row
  lcd.setCursor ( 0, 1 );            // go to the 2nd row
  lcd.print("v0.1.0 2023062701   "); // pad string with spaces for centering
  lcd.setCursor ( 0, 2 );            // go to the third row
  lcd.print("===================="); // pad with spaces for centering
  lcd.setCursor ( 0, 3 );            // go to the fourth row
  lcd.print("  www.nerdnet.com   ");

  delay(3000);
}

// main loop
void loop() 
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
  }
}
