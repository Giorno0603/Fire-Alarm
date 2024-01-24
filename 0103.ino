#include <Wire.h>
#include <BH1750.h>
#include <SimpleDHT.h>
#include <WiFi.h>
#include <HTTPClient.h>

//WiFi
const char* ssid = ""; //WiFi ID
const char* password = ""; //WiFi Password
const char* lineToken = "Lr7k3jHvBeaSqBzaij0rhwfr3HPXYHdknVyVDZlxMPV"; // Line Notify Token

// Motor A
int motor1Pin1 = 27; 
int motor1Pin2 = 26;

// Motor B
int motor2Pin1 = 32; 
int motor2Pin2 = 33; 

//Gas
const int GasPin = 36;
const int GasThreshold = 1500;

//Light
BH1750 lightMeter;
int LightThreshold = 100;

//DHT11
int pinDHT11 = 16;
SimpleDHT11 dht11(pinDHT11);
int TempThreshold = 30;



void setup() 
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  
  pinMode(GasPin, INPUT);
  pinMode(motor1Pin1,OUTPUT);
  pinMode(motor1Pin2,OUTPUT);
  pinMode(motor2Pin1,OUTPUT);
  pinMode(motor2Pin2,OUTPUT);
  pinMode(pinDHT11,INPUT);
  
  digitalWrite(motor1Pin1,LOW);
  digitalWrite(motor1Pin2,LOW);
  digitalWrite(motor2Pin1,LOW);
  digitalWrite(motor2Pin2,LOW);

  Wire.begin();

  lightMeter.begin();
}



void loop() 
{
  int GasValue = analogRead(GasPin);
  float lux = lightMeter.readLightLevel();
  
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }
  delay(1500);

  if (GasValue > GasThreshold) {
    sendLineNotify("煙霧太大了");
    digitalWrite(motor1Pin1,HIGH);
    digitalWrite(motor1Pin2,LOW);
  }
  else {
    digitalWrite(motor1Pin1,LOW);
    digitalWrite(motor1Pin2,LOW);
  }
  delay(1000);

  if (temperature > TempThreshold && lux > LightThreshold){
    digitalWrite(motor2Pin1,HIGH);
    digitalWrite(motor2Pin2,LOW);
    sendLineNotify("發生火災");
  }
  else{
    digitalWrite(motor2Pin1,LOW);
    digitalWrite(motor2Pin2,LOW);
  }
}



void sendLineNotify(String message) 
{
  HTTPClient http;
  String url = "https://notify-api.line.me/api/notify";
  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Authorization", "Bearer " + String(lineToken));
  int httpCode = http.POST("message=" + message);
  http.end();
}
