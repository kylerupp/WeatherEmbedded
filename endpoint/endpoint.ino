#include <CommonDef.h>
#include <DHT.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <math.h>

#define DHTPIN 4
#define DHTTYPE DHT11
#define FIRMWARE "0.0.1"

DHT dht(DHTPIN, DHTTYPE);
HTTPClient http;
WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(10);

  dht.begin();

  WiFi.begin(SSID_NET, SSID_KEY);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  String mac = WiFi.macAddress();
  mac.replace(':', '_');

  http.begin(client, "http://" SERVER "/endpoint/" + mac);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.GET();
  if(httpCode == 404) {
    http.POST("{\"mac\":\"" + mac + "\"}");
    http.end();
    
    http.begin(client, "http://" SERVER "/endpoint/" + mac + "/firmware");
    http.addHeader("Content-Type", "application/json");
    http.POST("{\"firmware\":\"" FIRMWARE "\"}");
  } else {
    http.PATCH("");   
  }

  http.end();

  int temp = round(dht.readTemperature(true, true));
  //float temp = DHT.temperature;
  int humidity = (int) dht.readHumidity();
  //int humidity = 1;

  http.begin(client, "http://" SERVER "/data");
  http.addHeader("Content-Type", "application/json");
  http.POST("{\"mac\":\"" + mac + "\",\"temp\":" + temp + ",\"humidity\":" + humidity + "}");
  http.end();
  
  // Make sure D0 is connected to RST for reboot
  ESP.deepSleep(30e6);
}

void loop() {
}
