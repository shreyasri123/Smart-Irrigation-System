 #include "DHT.h"  // Including library for dht
 
#include <ESP8266WiFi.h>
 
String apiKey = "1GIRLVD3J64LG132";     //  Enter your Write API key from ThingSpeak
 
const char *ssid =  "OPPO A9 2020";     // replace with your wifi ssid and wpa2 key
const char *pass =  "zzzzzzzz";
const char* server = "api.thingspeak.com";
const int sensor_pin = A0;
const int trigPin = 12;
const int echoPin = 14;
const int pump = D2;
int pump_status = 0;

//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

DHT dht;
WiFiClient client;
 
void setup() 
{
        dht.setup(D0); // data pin D1
       Serial.begin(9600);
       delay(10);
       pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
       pinMode(echoPin, INPUT); // Sets the echoPin as an Input
       pinMode(pump, OUTPUT); //  acts as pump connection pin
       digitalWrite(pump, LOW); // Initially make pump off
      // dht.begin();
 
       Serial.println("Connecting to ");
       Serial.println(ssid);
 
 
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
 
}
 
void loop() 
{
  
      float h = dht.getHumidity();
      float t = dht.getTemperature();
      float moisture_percentage = ( 100.00 - ( (analogRead(sensor_pin)/1023.00) * 100.00 ) );
      if (moisture_percentage < 0)
      {
        moisture_percentage = 0;
      }
      if (moisture_percentage == 0)
      {
        digitalWrite(pump, HIGH);
        pump_status = 1;
      }
      else 
      {
        digitalWrite(pump, LOW);
        pump_status = 0;
      }
              if (isnan(h) || isnan(t)) 
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }
 
                         if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                        // Clears the trigPin
                        digitalWrite(trigPin, LOW);
                        delayMicroseconds(2);
                        // Sets the trigPin on HIGH state for 10 micro seconds
                        digitalWrite(trigPin, HIGH);
                        delayMicroseconds(10);
                        digitalWrite(trigPin, LOW);
  
                        // Reads the echoPin, returns the sound wave travel time in microseconds
                        duration = pulseIn(echoPin, HIGH);
  
                        // Calculate the distance
                        distanceCm = duration * SOUND_VELOCITY/2;
  
                        // Convert to inches
                        distanceInch = distanceCm * CM_TO_INCH;
  
                        // Prints the distance on the Serial Monitor
                        Serial.print("Distance (cm): ");
                        Serial.println(distanceCm);
                        Serial.print("Distance (inch): ");
                        Serial.println(distanceInch);
                        Serial.print("Pump Status: ");
                        Serial.println(pump_status);
                        delay(100);
                            
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(t);
                             postStr +="&field2=";
                             postStr += String(h);
                             postStr +="&field3=";
                             postStr += String(moisture_percentage);
                             postStr +="&field4=";
                             postStr += String(distanceInch);
                             postStr +="&field5=";
                             postStr += String(pump_status);
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
 
                             Serial.print("Temperature: ");
                             Serial.print(t);
                             Serial.print(" degrees Celcius, Humidity: ");
                             Serial.print(h);
                             Serial.println("%. Send to Thingspeak.");
                             
                             Serial.print("Soil Moisture(in Percentage) = ");
                             Serial.print(moisture_percentage);
                             Serial.println("%");

                             delay(1000);
                             
                        }
          client.stop();
 
          Serial.println("Waiting...");
  
  // thingspeak needs minimum 15 sec delay between updates
  delay(1000);
}
