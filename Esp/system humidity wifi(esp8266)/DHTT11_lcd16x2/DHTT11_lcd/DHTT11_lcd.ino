#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// WiFi credentials
const char* ssid = "Fw";
const char* password = "191601004";

// DHT setup
#define DHTPIN 12       // D6 (tránh trùng SDA)
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // Địa chỉ LCD thường là 0x27 hoặc 0x3F

// Web server
AsyncWebServer server(80);

// Global data
float t = 0.0, h = 0.0;
unsigned long previousMillis = 0;
const long interval = 2000; // 2 giây

// HTML giao diện đơn giản
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head><meta charset="UTF-8"><title>ESP8266 DHT11</title></head>
<body style="text-align:center;font-family:sans-serif;">
  <h2>ESP8266 DHT11 Data</h2>
  <p>Nhiệt độ: <span id="temperature">%TEMPERATURE%</span> &deg;C</p>
  <p>Độ ẩm: <span id="humidity">%HUMIDITY%</span> %</p>
  <script>
    setInterval(() => {
      fetch('/temperature').then(r => r.text()).then(t => document.getElementById("temperature").innerHTML = t);
      fetch('/humidity').then(r => r.text()).then(h => document.getElementById("humidity").innerHTML = h);
    }, 2000);
  </script>
</body>
</html>
)rawliteral";

String processor(const String& var) {
  if (var == "TEMPERATURE") return String(t, 1);
  if (var == "HUMIDITY") return String(h, 1);
  return "";
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  lcd.init();
  lcd.backlight();

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  Serial.print("Đang kết nối WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nĐã kết nối! IP: ");
  Serial.println(WiFi.localIP());

  // Hiển thị thông báo WiFi lên LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi connected");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(3000); // Hiển thị trong 3 giây

  // Thiết lập các route cho Web server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(t, 1));
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(h, 1));
  });

  server.begin();
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    float newT = dht.readTemperature();
    float newH = dht.readHumidity();
    
    if (!isnan(newT)) t = newT;
    if (!isnan(newH)) h = newH;

    // In ra Serial
    Serial.printf("T: %.1f°C, H: %.1f%%\n", t, h);

    // Hiển thị LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Nhiet do: ");
    lcd.print(t, 1);
    lcd.print((char)223); // ký tự độ °
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("Do am: ");
    lcd.print(h, 1);
    lcd.print("%");
  }
}
