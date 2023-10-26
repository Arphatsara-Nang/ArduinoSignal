#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

// กำหนดออบเจ็กต์เซิร์ฟเวอร์และ DHT sensor
ESP8266WebServer server(80);
const int ledPin = D6;
DHT dht11(D4, DHT11);

// ฟังก์ชันเชื่อมต่อ WiFi
void initWifi(const char* ssid, const char* password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}


// ฟังก์ชันจัดการหน้าหลัก
// ส่วนอื่นๆ ของโค้ดคงเดิม

// Function to handle the root path
void handleRoot() {
  float temperature = dht11.readTemperature();
  float humidity = dht11.readHumidity();

  // Create HTML for the web page
  String html = "<html><head>"
                "<link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css'>"
                "</head><body><div class='container' style='text-align: center; margin-top: 20px;'>"
                "<h2 class='text-primary'>ESP8266</h2>"
                "<h3>Web Server</h3>"
                "<p>Humidity: <span id='humidity'>" + String(humidity) + " %</span></p>"
                "<p>Temperature: <span id='temperature'>" + String(temperature) + " C</span></p>"
                "<p>LED Status: ";

  if (digitalRead(ledPin) == HIGH) {
    html += "<span class='text-success'>On</span>";
  } else {
    html += "<span class='text-danger'>Off</span>";
  }

  html += "</p>"
          "<form action='/toggle' method='get'>"
          "<input type='submit' class='btn btn-warning' value='Toggle LED'>"
          "</form>"
          "</div>"
          // JavaScript remains unchanged
          "</body></html>";

  server.send(200, "text/html", html);
}


// ส่วนอื่นๆ ของโค้ดคงเดิม


void handleToggle() {
  int currentState = digitalRead(ledPin);
  digitalWrite(ledPin, !currentState);
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}


// ฟังก์ชันส่งข้อมูลอุณหภูมิและความชื้น
void handleData() {
  float temperature = dht11.readTemperature();
  float humidity = dht11.readHumidity();
  String data = "{\"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + "}";
  server.send(200, "application/json", data);
}

// ฟังก์ชัน setup ที่จะถูกเรียกใช้งานเมื่อบอร์ดเริ่มทำงาน
void setup() {
  Serial.begin(115200);
  initWifi("JaNa", "1234567890");
  pinMode(ledPin, OUTPUT);
  dht11.begin();

  server.on("/", HTTP_GET, handleRoot);
  server.on("/toggle", HTTP_GET, handleToggle);
  server.on("/data", HTTP_GET, handleData);

  server.begin();
  Serial.println("HTTP server started");
}

// ลูปหลักของโปรแกรม
void loop() {
  server.handleClient();
}
