#include <ESP8266WiFi.h> 
#include <ESP8266HTTPClient.h> 
#include <MFRC522.h> 
#include <SPI.h> 
 
#define SERVER_IP "" //Add the server IP or domain 
 
#ifndef STASSID 
#define STASSID "your_wifi_ssid" 
#define STAPSK "your_wifi_password" 
#define SS_PIN 4 
#define RST_PIN 5 
#endif 
 
MFRC522 mfrc522(SS_PIN, RST_PIN); 
 
void setup() {   
  pinMode(LED_BUILTIN, OUTPUT);   
  Serial.begin(115200);   
  //Wifi Stuff here ...      
  WiFi.begin(STASSID, STAPSK); 
 
  while(WiFi.status() != WL_CONNECTED) {     
  digitalWrite(LED_BUILTIN, 0);     
  Serial.print(".");     
  delay(250);     
  digitalWrite(LED_BUILTIN, 1);     
  delay(250);   
  } 
 
  digitalWrite(LED_BUILTIN, 1);      
  Serial.print("Connected to: ");   
  Serial.println(STASSID);   
  Serial.print("IP Adress: ");   
  Serial.println(WiFi.localIP()); 
 
  //MFRC522 Stuff here... 
 
  SPI.begin();   mfrc522.PCD_Init();   
  Serial.println("Approchez votre carte RFID: "); 
  }  
 
 void loop() {   //Nothing should work before WiFi connection is established 
 
  if (WiFi.status() == WL_CONNECTED) {     
  //Create an instance of WiFiClient and an instance of HTTPClient class     
  WiFiClient client;     
  HTTPClient http;          
  
  //If there's no card present     
  if (!mfrc522.PICC_IsNewCardPresent()) {       
    return;     
  }     
  if (!mfrc522.PICC_ReadCardSerial()) {       
    return;     
  } 
 
    //Blink the LED_BUILTIN when a Card is deteceted     
  digitalWrite(LED_BUILTIN, 0);     
  delay(250);     
  digitalWrite(LED_BUILTIN, 1);          
  Serial.print("Card UID: ");     
  String uid = "";     
  byte letter; 
 
    for (byte i = 0; i < mfrc522.uid.size; i++) {       
    uid.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));       
    uid.concat(String(mfrc522.uid.uidByte[i], HEX));     
  }     
    uid.toUpperCase();     
    Serial.println(uid); 
 
    //Sending the Card UID to the WebServer 
 
     Serial.println("HTTP connection begins ... "); 
 
     //Connect to the server and set Header of the POST request      
   http.begin(client, "http://" SERVER_IP "/"); 
   http.addHeader("Content-Type", "application/json"); 
 
     //Sending the post request and get the HTTP response status code 
 
     int statusCode = http.POST("{\"uid\":\"" + uid + "\"}"); 
 
     // statusCode will be negative on error     
   if (statusCode > 0) { 
          
      //HTTP header has been send and Server response header has been handled       
      Serial.printf("[HTTP] POST... code: %d\n", statusCode); 
 
    //file found at server       
    if (statusCode == HTTP_CODE_OK) {         
      const String& payload = http.getString();         
      Serial.println("received payload:\n<<");         
      Serial.println(payload);         
      Serial.println(">>");         
      digitalWrite(LED_BUILTIN, 0);         
      delay(3000);         
      digitalWrite(LED_BUILTIN, 1);       
      }     
    }      
    else { 
      Serial.printf("[HTTP] POST... failed, error: %s\n",   
      http.errorToString(statusCode).c_str());       
      digitalWrite(LED_BUILTIN, 0);       
      delay(3000);       
      digitalWrite(LED_BUILTIN, 1);     
    }  
 
  http .end();        
    } 
  } 
