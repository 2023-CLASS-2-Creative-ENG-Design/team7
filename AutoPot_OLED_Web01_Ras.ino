/*  ### 수분 자동 공급 IoT 화분 - IoT 관리 버전 code - ### 
 *  1. WiFi 연결을 통해 수분의 데이터를 기록하고 웹브라우저 또는 전용앱을 통해 데이터를 확인 할 수 있는 코드입니다.
 *  2. WiFi 연결이 끊겨도 수분의 자동공급 기능은 작동합니다.
 *  3. 사용하고자 하는 OLED종류나 OLED드라이버가 따로 있다면 해당부분 코드를 수정해 주세요.
 *  4. 아래 코드 중에서(#A부분) 제작품을 테스트해보고 펌프를 작동하게 할 적절한 센서값을 변경하면 됩니다.  
 *  5. 코드 컴파일시 발생하는 에러의 상당수는 라이브러리가 일치하지 않아서 발생하게 됩니다. 라이브러리 이름이 같아도 
 *     버전과 내용이 다르면 에러가 발생하니 Rasino IoT에서 제공하는 라이브러리를 설치해 보시기 바랍니다. 
 *  6. '파일 》환경설정' 메뉴 '추가 보드...URLs'부분에 ' http://arduino.esp8266.com/stable/package_esp8266com_index.json ' 입력
 *  제작과정의 좀더 상세한 내용과 설명은 http://rasino.tistory.com/347 를 참조 하세요   
 */
 
#include <ESP8266WiFi.h> //제시한 라이브러리를 설치 후에도 이부분에 에러가 나면,ESP8266환경설정과,보드를 D1 mini(esp8266보드 계열)로 선택했는지 확인.
#include <WiFiUdp.h>
#include "mySSID.h"
#include "Images.h"
#include "SSD1306Wire.h"           // I2C용 OLED 라이브러리 사용

// SSD1306Wire  display(0x3c, 2, 0);  // esp8266-01 과 OLED를 연결할 경우 GPIO (SDA, SCK) 포트 번호
SSD1306Wire  display(0x3c, 4, 5);  // D1 mini 와 OLED를 연결할 경우 GPIO (SDA, SCL) 포트 번호
String apiKey = thingSpeak_Pot;  // ThingSpeak의 Write API Key 입력 (사이트 가입후 받은 번호를 직접 입력) 
char ssid[] = mySSID1;  // 파일 추가로 입력해도 되고, 여기에 WiFi이름을 직접 입력해도 됩니다.
char pass[] = myPASS1;  // WiFi 비번 입력.
const char* server = "api.thingspeak.com";
WiFiClient client;    // client 시작하기

int motorA1 =  D6;   // L9110S 모듈 관련 변수 선언
int motorA2  = D7;   // L9110S 모듈 관련 변수 선언
int sensorVal = A0;

void setup(){
    display.init();
    display.clear();
    display.flipScreenVertically();  // 스크린 상하 플립
    display.drawXbm(0, 0, RasLogo_W, RasLogo_H, RasLogo);       
    display.display(); 
    delay(2000);
    display.clear();
    display.drawRect(0, 0, 128, 64);
    display.setFont(ArialMT_Plain_16);  // ArialMT_Plain폰트는 10, 16, 24
//    display.setTextAlignment(TEXT_ALIGN_CENTER);    // LEFT , RIGHT
    display.drawString(5, 10, "Auto Flower Pot");        
    display.setFont(ArialMT_Plain_10);  // ArialMT_Plain폰트는 10, 16, 24    
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
    sensorVal = map(analogRead(A0), 450, 1023, 100, 0);  // 토양센서 측정범위 확인 후 0~9의 값으로 맵핑값 저장
//    Serial.print("Asensor = ");
//    Serial.println(analogRead(A0));  // 0(건조) ~ 100(습함) 값 출력    
    display.drawXbm(0, 0, flowerPot_W, flowerPot_H, flowerPot);
    display.setFont(ArialMT_Plain_16);  // ArialMT_Plain폰트는 10, 16, 24  
    display.drawString(50, 0, "Pot :");
    display.drawString(90, 0, String(sensorVal));    
    if ( sensorVal < 52) {
      display.setFont(ArialMT_Plain_16);  // ArialMT_Plain폰트는 10, 16, 24  
//      Serial.print("Pot is Very Dry !");
//      Serial.print("Now Pumping...");
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

    if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com   
    {  
       String sendData = apiKey+"&field1="+String(sensorVal)+"\r\n\r\n";      
       //Serial.println(sendData);
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
