/*
Dragan Kovacevic
ESP8266 Garage Door Relay Controller
*/

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
//#include <Servo.h>
//#include <EEPROM.h>

#define WIFI_SSID "HIDDEN"
#define WIFI_PASS "HIDDEN"

#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "HIDDEN"
#define MQTT_PASS "HIDDEN"
#define IO_USERNAME "HIDDEN"
#define IO_KEY "HIDDEN"


WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, IO_KEY);

Adafruit_MQTT_Subscribe opencloseRight = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/right-garage-door-feed");
Adafruit_MQTT_Subscribe opencloseLeft = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/left-garage-door-feed");

const int sleepSeconds = 5;

void setup()
{
  Serial.begin(115200);
  
  //Subscribe to the onoff topic
  mqtt.subscribe(&opencloseRight);
  mqtt.subscribe(&opencloseLeft);
  
  //Connect to WiFi
  Serial.print("\n\nConnecting Wifi... ");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(1500); }

  Serial.println("Connected OK!");
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(5,OUTPUT);//left door
  pinMode(4,OUTPUT);//right door
  digitalWrite(5,HIGH);
  digitalWrite(4,HIGH);
}

void controlDoors()
{
  //Connect/Reconnect to MQTT
  MQTT_connect();

  //wait up to 5 seconds for subscription to update
  Adafruit_MQTT_Subscribe * subscription;
  Serial.println("reading sub status");
  while(subscription = mqtt.readSubscription(1000)){
    if (subscription == &opencloseRight)
    { 
      //If the new value is  "ON", turn the light on.
      if (strcmp((char*) opencloseRight.lastread, "0") == 0)
      {
        Serial.println("0 was read from MQTT broker");
        //active low logic
        digitalWrite(5,LOW);
        delay(100);
        digitalWrite(5,HIGH);
        Serial.printf("Clicked Right Garage Button");
      }
      else {
        
        digitalWrite(5,HIGH);
        }    
    }
    
    if (subscription == &opencloseLeft)
    {
      //If the new value is  "ON", turn the light on.
      if (strcmp((char*) opencloseLeft.lastread, "0") == 0)
      {
        Serial.println("0 was read from MQTT broker");
        //active low logic
        digitalWrite(4,LOW);
        delay(100);
        digitalWrite(4,HIGH);
        Serial.printf("Clicked Left Garage Button");
      }
      else {
        
        digitalWrite(4,HIGH);
        }    
    }
    
  if(! mqtt.ping()){
    //Serial.println("MQTT Ping failed.");
  }
  }
}

void loop()
{
  controlDoors();
  
  //Serial.printf("Sleeping deep for %i seconds...", sleepSeconds);
  //ESP.deepSleep(sleepSeconds * 1000000); //enable this to enable hibernation, though it causes significant delays
}



void MQTT_connect() 
{
  int8_t ret;
  // Stop if already connected
  if (mqtt.connected())
  {
    return;
  }

  //Serial.print("Connecting to MQTT... ");
  uint8_t retries = 2;
  while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
  { 
    //Serial.println(mqtt.connectErrorString(ret));
    //Serial.println("Retrying MQTT connection in 1 seconds...");
    mqtt.disconnect();
    delay(1000);  // wait 1 seconds
    retries--;
    if (retries == 0) 
    {
      // basically die and wait for WDT to reset me
      ESP.restart();
      while (1);
    }
  }
  //Serial.println("MQTT Connected!");
}



  
