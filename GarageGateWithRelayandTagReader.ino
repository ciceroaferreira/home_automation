#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h> 
#include "DHT.h"     // including the library of DHT11 temperature and humidity sensor

#define DHTPIN 4 //Pino digital D2 (GPIO5) conectado ao DHT11
#define DHTTYPE DHT11 //Tipo do sensor DHT11
#define CLIENT_ID       "Hal_GATE" 

// For ESP32 Microcontroller
#define RST_PIN         0
#define SS_PIN          2

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
 
void callback(char* topic, byte* payload, unsigned int length);

//Wifi Settings
const char* ssid = "WIFI_NAME";
const char* password =  "WIFI_PASSWORD";

unsigned long previousMillis = 0;
unsigned long interval = 30000;

//MQTT Settings
const char* mqttServer = "MQTT_IP";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(mqttServer, mqttPort, callback, espClient);
//PubSubClient client(espClient);
//DEPOIS QUANDO ACOPLAR O SENSOR DE TEMPERATURA USO AQUI
DHT dht(DHTPIN, DHTTYPE);
const int relay = 16;


void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}


void setup() {

  Serial.begin(115200);
  SPI.begin();             //Inicia SPI  
  
  //pinMode(analogMQ7, INPUT);  
  pinMode(relay, OUTPUT); 
  
  //WiFi.begin(ssid, password);
  initWiFi();

  digitalWrite(relay,LOW);  
   
  client.setClient(espClient);
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  //MQTT Connection
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect(CLIENT_ID)) {
 
      Serial.println("MQTT connected");
      client.subscribe("garage/door"); 
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
    dht.begin();

    Serial.println(F("Starting MFRC22..."));
    mfrc522.PCD_Init(); // Init MFRC522
    mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
    Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  
    Serial.println(F("Started..."));
  

    Serial.println("Approximate your card to the reader...");
    Serial.println();
  }     
}


void loop() {
  
  // Read
  // Check the Wifi Connection, if disconnected reconnect...
  /*
  unsigned long currentMillis = millis();
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
*/
  //Get Information from sensors
 sendData();

 client.subscribe("garage/door/");
 
 
 client.loop();
 //MUST delay to allow ESP8266 WIFI functions to run
 //delay(10); 
}

void sendData() {
  char msgBuffer[20];   

    float humidity = dht.readHumidity();/* Get humidity value */
    float temperature = dht.readTemperature();/* Get temperature value */
 
  if (client.connect(CLIENT_ID)) {
      //Serial.println(temperature);
      //Serial.println(humidity);
      client.publish("home/gate/temp", dtostrf(temperature, 6, 2, msgBuffer));
      client.publish("home/gate/humi", dtostrf(humidity, 6, 2, msgBuffer));         
  }

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }


  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "E9 E0 2E C3" || content.substring(1) == "DA AF A6 80" || content.substring(1) == "A9 3D 6B E4" || content.substring(1) == "E9 2F 05 E5") //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Authorized access");
    Serial.println();
    digitalWrite(relay, HIGH);
    delay(3000);
    digitalWrite(relay, LOW);
  }
 
 else   {
    Serial.println(" Access denied");
    delay(3000);
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
    //client.publish("garage/door", "1");
    digitalWrite(relay, HIGH);    
  } else if (payload[0] == 48)      // Message "0" in ASCII (turn output OFF)
  {
    //client.publish("garage/door", "0");
    digitalWrite(relay, LOW);
  } else if (payload[0] == 50)
  {
    //client.publish("home/br/nb/ip", ip.c_str());// publish IP nr
  } else {
    Serial.println("Unknown value");
    client.publish("home/br/nb", "Syntax Error");
  }

}
