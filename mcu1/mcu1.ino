#define BLYNK_TEMPLATE_ID "TMPL6IOqF_tDR"
#define BLYNK_TEMPLATE_NAME "Test"
#define BLYNK_AUTH_TOKEN "ZV91ESQgZblFTHYOb12t787cP2J24gUm"
#include <BlynkSimpleEsp8266.h>

//untuk http
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
WiFiClient client;

#include<SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial dataSerial(12, 13); //(TX, RX);

char ssid[] = "0000";
char pass[] = "tadiganti";
char auth[] = BLYNK_AUTH_TOKEN;
const char *host = "192.168.149.233";

unsigned long previousMillis = 0;
const long interval = 2000;

float temp1, voltage1, current1, energy1;
String arrData[4];

void setup() {
  delay(1000);
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_STA);
  Serial.begin(9600);
  dataSerial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  setDisplay();
  WiFi.begin(ssid, pass);
  while(WiFi.status() !=WL_CONNECTED){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("STATUS...");
    lcd.setCursor(0,1);
    lcd.print("Connect Success");
    delay(2000);
  }
}

void loop() {
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;

   String data = "";
  while(dataSerial.available()>0){
    data += char(dataSerial.read());
  }

  data.trim();

  if(data != ""){
    int index = 0;
    for(int i=0; i<= data.length(); i++){
      char delimiter = '#';
      if(data[i] != delimiter){
        arrData[index] += data[i];
      }
      else{
        index++;
      }
    }

    if(index == 3){
      Serial.println("Temperature    : " + arrData[0] + "C");
      Serial.println("DC Voltage     : " + arrData[1] + "V");
      Serial.println("DC Current     : " + arrData[2] + "A");
      Serial.println("Total Energy   : " + arrData[3] + "Watt");
      Serial.println();
    }

    temp1      = arrData[0].toFloat();
    voltage1   = arrData[1].toFloat();
    current1   = arrData[2].toFloat();
    energy1    = arrData[3].toFloat();

    arrData[0] = "";
    arrData[1] = "";
    arrData[2] = "";
    arrData[3] = "";

    Blynk.run();
    Blynk.virtualWrite(V0, voltage1);
    Blynk.virtualWrite(V1, current1);
    Blynk.virtualWrite(V2, energy1);
    Blynk.virtualWrite(V5, temp1);
  
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("V:");
    lcd.setCursor(2,0);
    lcd.print(voltage1);
    lcd.setCursor(8,0);
    lcd.print("A:");
    lcd.setCursor(11,0);
    lcd.print(current1);
    lcd.setCursor(0,1);
    lcd.print("C:");
    lcd.setCursor(2,1);
    lcd.print(temp1);
    if(temp1 > 25){
      lcd.setCursor(8,1);
      lcd.print("FAN ON");
    }
    else{
      lcd.setCursor(8,1);
      lcd.print("FAN OFF");
    }

    HTTPClient http;
    String postData, temp2, voltage2, current2, energy2, link;

    temp2 = String(temp1);
    voltage2 = String(voltage1);
    current2 = String(current1);
    energy2 = String(energy1);

    postData = "&status1=" + temp2 + "&status2=" + voltage2 + "&status3=" + current2 + "&status4=" + energy2;
    link = "http://192.168.149.233/solarpanel/postdemo.php";
    http.begin(client, link);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpCode = http.POST(postData);
    String payload = http.getString();

    Serial.println(httpCode);
    Serial.println(payload);
    Serial.println();

    http.end();
    
    delay(2000);
    }
  dataSerial.println("yes"); 
  }
}

void setDisplay(){
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Welcome...");
  lcd.setCursor(0,1);
  lcd.print("Connected Wifi");
  delay(1000);
}
