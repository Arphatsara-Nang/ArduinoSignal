#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "NXNG0917";
const char* password = "========";
const char* serverAddress = "http://10.1.1.94:3000/sensors";

WiFiClient client;
HTTPClient http;
DHT dht(D4, DHT11);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  dht.begin();
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("WiFi connected");
}

void loop() {
  static unsigned long lastTime = 0;
  unsigned long timerDelay = 15000;

  if (millis() - lastTime > timerDelay) {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor");
    } else {
      Serial.print("Humidity: ");
      Serial.println(humidity);
      Serial.print("Temperature: ");
      Serial.println(temperature);

      // Create a JSON document
      DynamicJsonDocument jsonDocument(200);
      jsonDocument["hum"] = humidity;
      jsonDocument["temp"] = temperature;
      jsonDocument["timestamp"] = millis();

      // Serialize the JSON document to a string
      String jsonData;
      serializeJson(jsonDocument, jsonData);

      // Send data to the server
      sendData(jsonData);
    }

    lastTime = millis();
  }
}

void sendData(String data) {
  http.begin(client, serverAddress);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(10000);

  int httpResponseCode = http.POST(data);

  if (httpResponseCode > 0) {
    Serial.println("HTTP Response code: " + String(httpResponseCode));
  } else {
    Serial.println("Error on sending POST: " + String(httpResponseCode));
  }

  http.end();
}