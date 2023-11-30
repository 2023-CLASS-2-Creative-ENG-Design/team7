
 
#include <ESP8266WiFi.h> 
#include <WiFiUdp.h>
#include "mySSID.h"
#include "Images.h"
#include "SSD1306Wire.h"           

 
SSD1306Wire  display(0x3c, 4, 5); 
String apiKey = thingSpeak_Pot;  
char ssid[] = mySSID1;  
char pass[] = myPASS1;  
const char* server = "api.thingspeak.com";
WiFiClient client;   

int motorA1 =  D6;   
int motorA2  = D7;  
int sensorVal = A0;

void setup(){
    display.init();
    display.clear();
    display.flipScreenVertically(); 
    display.drawXbm(0, 0, RasLogo_W, RasLogo_H, RasLogo);       
    display.display(); 
    delay(2000);
    display.clear();
    display.drawRect(0, 0, 128, 64);
    display.setFont(ArialMT_Plain_16); 

    display.drawString(5, 10, "Auto Flower Pot");        
    display.setFont(ArialMT_Plain_10);    
    display.drawString(8, 36, "RasINO.TISTORY.COM");            
    display.drawString(11, 48, "YouTube:RasINO IOT");         
    display.display(); 
    delay(3500);    
    
    Serial.begin(9600);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print("*");
      display.clear();
      display.drawString(11, 48, "Connecting WiFi...");               
      display.display(); 
      delay(6000);
      break;
    }
    pinMode( motorA1 , OUTPUT); 
    pinMode( motorA2 , OUTPUT);   
    display.clear();
}

void loop () {
    sensorVal = map(analogRead(A0), 450, 1023, 100, 0); 
   
    display.drawXbm(0, 0, flowerPot_W, flowerPot_H, flowerPot);
    display.setFont(ArialMT_Plain_16);  
    display.drawString(50, 0, "Pot :");
    display.drawString(90, 0, String(sensorVal));    
    if ( sensorVal < 52) {
      display.setFont(ArialMT_Plain_16);  

      digitalWrite(motorA1, HIGH);
      digitalWrite(motorA2, LOW);
      display.drawString(50, 30, "Pot Dri !");
      display.drawString(50, 45, "Pumping...");
      display.display();
      delay(10);    
    } else {
      digitalWrite(motorA1, LOW);
      digitalWrite(motorA2, LOW);
      display.drawString(50, 30, "Pot OK!");
      display.drawString(50, 45, "Sleeping...");
      display.display();
      delay(10);
    }
    display.clear();

    if (client.connect(server,80))   
    {  
       String sendData = apiKey+"&field1="+String(sensorVal)+"\r\n\r\n";      
       client.print("POST /update HTTP/1.1\n");
       client.print("Host: api.thingspeak.com\n");
       client.print("Connection: close\n");
       client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
       client.print("Content-Type: application/x-www-form-urlencoded\n");
       client.print("Content-Length: ");
       client.print(sendData.length());
       client.print("\n\n");
       client.print(sendData);
       delay(10);
    }
}
