#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include "DHT.h"     // including the library of DHT11 temperature and humidity sensor

#define DHTPIN 4 //Pino digital D2 (GPIO5) conectado ao DHT11
#define DHTTYPE DHT11 //Tipo do sensor DHT11
//Include the library
#define pin 39 //Analog input 0 of your arduino
#define type 7 //MQ7
#define CLIENT_ID       "Hal_CO2" 
#define analogMQ7 A0      // Signal

void callback(char* topic, byte* payload, unsigned int length);

//Wifi Settings
const char* ssid = "WIFI_NAME";
const char* password =  "WIFI_PASSWORD";

//MQTT Settings
const char* mqttServer = "MQTT IP";
const int mqttPort = 1883;
const char* mqttUser = "MyUser";
const char* mqttPassword = "MyPasswd";
int MQ7sensorValue = 0;   // value read from the sensor

WiFiClient espClient;
PubSubClient client(mqttServer, mqttPort, callback, espClient);
//PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);
const int relay = 5;


void setup() {

  Serial.begin(115200);
  pinMode(analogMQ7, INPUT);
  pinMode(relay, OUTPUT); 
  
  WiFi.begin(ssid, password);

  //digitalWrite(relay, HIGH);
  //delay(5000);
  //digitalWrite(relay, LOW);
  //delay(5000);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setClient(espClient);
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect(CLIENT_ID)) {
 
      Serial.println("connected");
      client.subscribe("garage/light"); 
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
       dht.begin();
       //client.publish("garage/value", 0);
   
  }     
}


void loop() {
  // Read
   
 sendData();

 client.subscribe("garage/light/");

 
 client.loop();
 //MUST delay to allow ESP8266 WIFI functions to run
 //delay(10); 
}

void sendData() {
  char msgBuffer[20];   
    
    analogWrite(analogMQ7, HIGH); // HIGH = 255
    // heat for 1 min
    //delay(60000);
    //delay(5000);
    // now reducing the heating power: turn the heater to approx 1,4V
    analogWrite(analogMQ7, 71.4); // 255x1400/5000
    // heat for 90 sec
    //delay(90000);
    //delay(10000);
    
  // B) reading    
    // CO2 via MQ7: we need to read the sensor at 5V, but must not let it heat up. So hurry!
    analogWrite(analogMQ7, HIGH); 
    delay(50); // Getting an analog read apparently takes 100uSec
    MQ7sensorValue = analogRead(analogMQ7);          
    
  // C) print the results to the serial monitor
    //Serial.print("MQ-7 PPM: ");                       
    Serial.println(MQ7sensorValue);
  
  
         
  if (client.connect(CLIENT_ID)) {
    Serial.println("connected");    ;
    float humidity = dht.readHumidity();/* Get humidity value */
    float temperature = dht.readTemperature();/* Get temperature value */
    
    if (MQ7sensorValue <= 200)  
     { 
         Serial.println("Air-Quality: CO perfect");
         Serial.println(dtostrf(MQ7sensorValue, 6, 2, msgBuffer));
         Serial.println(temperature);
         Serial.println(humidity);
         client.publish("home/garage/temp", dtostrf(temperature, 6, 2, msgBuffer));
         client.publish("home/garage/humi", dtostrf(humidity, 6, 2, msgBuffer));             
         client.publish("garage/value", dtostrf(MQ7sensorValue, 6, 2, msgBuffer));
         client.publish("garage/message", "CO2 perfeito");             
     } 
     else if ((MQ7sensorValue > 200) || (MQ7sensorValue <= 800)) // || = or 
     { 
         
         Serial.println("Air-Quality: CO normal"); 
         Serial.println(dtostrf(MQ7sensorValue, 6, 2, msgBuffer));
         Serial.println(temperature);
         Serial.println(humidity);
         client.publish("home/garage/temp", dtostrf(temperature, 6, 2, msgBuffer));
         client.publish("home/garage/humi", dtostrf(humidity, 6, 2, msgBuffer));           
         client.publish("garage/value", dtostrf(MQ7sensorValue, 6, 2, msgBuffer));
         client.publish("garage/message", "CO2 Normal");         
       
     } 
     else if ((MQ7sensorValue > 800) || (MQ7sensorValue <= 1800)) 
     { 
         Serial.println("Air-Quality: CO high");       
         Serial.println(dtostrf(MQ7sensorValue, 6, 2, msgBuffer));
         client.publish("home/garage/temp", dtostrf(temperature, 6, 2, msgBuffer));
         client.publish("home/garage/humi", dtostrf(humidity, 6, 2, msgBuffer));        
         client.publish("garage/value", dtostrf(MQ7sensorValue, 6, 2, msgBuffer));
         client.publish("garage/message", "CO2 Alto");         
     } 
     else if (MQ7sensorValue > 1800)  
     {        
         Serial.println("Air-Quality: ALARM CO very high");
         Serial.println(dtostrf(MQ7sensorValue, 6, 2, msgBuffer));
         client.publish("home/garage/temp", dtostrf(temperature, 6, 2, msgBuffer));
         client.publish("home/garage/humi", dtostrf(humidity, 6, 2, msgBuffer));         
         client.publish("garage/value", dtostrf(MQ7sensorValue, 6, 2, msgBuffer));
         client.publish("garage/message", "PERIGO! CO2 Muito Alto");                
     } 
     else 
     {        
         Serial.println("MQ-7 - cant read any value - check the sensor!");        
     }    
  }
  
  // Wait
  //delay(5000);
}

void callback(char* topic, byte* payload, unsigned int length) {
  char msgBuffer[20];
  // I am only using one ascii character as command, so do not need to take an entire word as payload
  // However, if you want to send full word commands, uncomment the next line and use for string comparison
  // payload[length]='\0';// terminate string with 0
  
 
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");//MQTT_BROKER
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();
  Serial.println(payload[0]);

  // Examine only the first character of the message
  if (payload[0] == 49)             // Message "1" in ASCII (turn output ON)
  {
    //client.publish("garage/light", "1");
    digitalWrite(relay, HIGH);    
  } else if (payload[0] == 48)      // Message "0" in ASCII (turn output OFF)
  {
    //client.publish("garage/light", "0");
    digitalWrite(relay, LOW);
  } else if (payload[0] == 50)
  {
    //client.publish("home/br/nb/ip", ip.c_str());// publish IP nr
  } else {
    Serial.println("Unknown value");
    client.publish("home/br/nb", "Syntax Error");
  }

}
