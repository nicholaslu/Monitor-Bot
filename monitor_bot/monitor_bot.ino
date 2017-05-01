/**************************************************************************************************************************
   Monitor-Bot
   2017/5/1 nicholaslu
   Change token and varible id to your own
   Change Wi-Fi SSID and password to your own
   Moisture:  # 300 ~400 fully in water
              # 401 ~949 in the soil
              # 950 ~999 fully in air
   Required Aditional Boards Manager URLs for Arduino IDE:
                     http://arduino.esp8266.com/stable/package_esp8266com_index.json
   Required library: https://github.com/adafruit/DHT-sensor-library
                     https://github.com/adafruit/Adafruit_Sensor
                     https://github.com/ubidots/ubidots-nodemcu/archive/master.zip
   Reference:        http://blog.simtronyx.de/en/measurement-of-illuminance-with-a-bh1750fvi-breakout-board-gy-30-and-an-arduino-uno/
                     http://help.ubidots.com/connect-your-devices/connect-an-esp8266-nodemcu-10-to-ubidots
**************************************************************************************************************************/

#include <Wire.h>
#include <UbidotsMicroESP8266.h>
#include "DHT.h"
#include <SoftwareSerial.h>

int nap = 60000; // Upload data every 60 seconds
int BH1750_address = 0x23; // i2c Addresse
byte buff[2];

#define SOMO A0 // Soil moisture sensor connected to PIN A0 of NodeMCU 

#define DHTPIN D4 // DHT11 connected to PIN D4 of NodeMCU
#define DHTTYPE DHT11

#define SSID "NETGEAR64"
#define PASS "silkychair519"

#define TOKEN "iezgWhoTrRDeIdul7oiZJjcXcm1iUv"

Ubidots client(TOKEN);
DHT dht(DHTPIN, DHTTYPE);
void setup() {

  client.wifiConnection(SSID, PASS);

  Wire.begin();
  BH1750_Init(BH1750_address);

  dht.begin();

  delay(200);
  Serial.begin(115200);

}

void loop() {

  Serial.println("");
  Serial.println("light   temp    humi    moisture");
  float valf = 0;

  if (BH1750_Read(BH1750_address) == 2) {

    valf = ((buff[0] << 8) | buff[1]) / 1.2;

    if (valf < 0)Serial.print("> 65535");
    else Serial.print((int)valf, DEC);

    float light = valf;
    client.add("58eb763d7625420f62586aff" , light);

    Serial.print(" lx");
    Serial.print(" / ");
  }

  delay(2000);
  float temp = dht.readTemperature();
  client.add("58eb75c47625420f64aa4c46", temp);

  float humi = dht.readHumidity();
  client.add("58eb76b57625420f66f21e06", humi);

  float moisture = analogRead(SOMO);
  client.add("58eb76a67625420f64aa58a9", moisture);

  Serial.print(temp);
  Serial.print(" / ");
  Serial.print(humi);
  Serial.print(" / ");
  Serial.println(moisture);

  client.sendAll();

  delay(nap);
}

void BH1750_Init(int address) {

  Wire.beginTransmission(address);
  Wire.write(0x10); // 1 [lux] aufloesung
  Wire.endTransmission();
}

byte BH1750_Read(int address) {
  byte i = 0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while (Wire.available()) {
    buff[i] = Wire.read();
    i++;
  }
  Wire.endTransmission();
  return i;
}

