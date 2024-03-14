#include <DHT.h>
#include <ACS712.h>

ACS712 sensorCurrent(ACS712_30A, A1);

#define DHTPIN 9
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

int relayPin = 8;

int datavolt = A0;
float vout = 0.0;
float vout1 = 0.0;
float R1 = 19000.0;
float R2 = 7500.0;
int value = 0;

void setup (){
  Serial.begin(9600);
  dht.begin();
  pinMode(relayPin, OUTPUT);
  sensorCurrent.calibrate();
}

void loop(){
  String request = "";
  while(Serial.available()>0){
    request += char(Serial.read());
  }
  request.trim();

  if(request == "yes"){
    sendData();
  }
  request = "";
  delay(500);
}

void sendData(){
  value = analogRead(datavolt);
  vout = value * (5.0 / 1024);
  vout1 = vout/(R2/(R1+R1));
  float temp0   = dht.readTemperature();
  float currentDC = sensorCurrent.getCurrentDC();
  if(currentDC < 0){
    currentDC = 0;
  }
  float energy0 = vout1*currentDC;
  String dataSend  = String(temp0)+"#"+String(vout1,2)+"#"+String(currentDC)+"#"+String(energy0);
  Serial.println(dataSend);
  if (temp0>25){
    digitalWrite(relayPin, HIGH);
  }
  else{
    digitalWrite(relayPin,LOW);
  }
}
