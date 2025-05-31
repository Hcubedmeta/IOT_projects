#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
// #include <Adafruit_Sensor.h> 
#include <DHT.h>
#include <Servo.h>

const char* ssid = "Redmi Note 12";
const char* password = "namnamnam";

#define DHTPIN 4       // D2
#define DHTTYPE DHT11
#define LED_PIN 2      // D4 (LED tích hợp ESP8266)

Servo myServo;
int servoAngle = 90;     // góc ban đầu
#define SERVO_PIN 14     // D5 trên NodeMCU

DHT dht(DHTPIN, DHTTYPE);
AsyncWebServer server(80);

float t = 0.0, h = 0.0;
bool ledState = false;
unsigned long previousMillis = 0;
const long interval = 2000; // 2s

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP8266 DHT & LED</title>
  <style>
    body { font-family: Arial; text-align: center; margin-top: 50px; }
    h2 { font-size: 2.5rem; }
    p { font-size: 2rem; }
    button {
      padding: 15px 30px;
      font-size: 1.2rem;
      margin: 10px;
      cursor: pointer;
    }
    .on { background-color: green; color: white; }
    .off { background-color: red; color: white; }
  </style>
</head>
<body>
  <h2>ESP8266 DHT & LED Control</h2>
  <p>Nhiệt độ: <span id="temperature">%TEMPERATURE%</span> &deg;C</p>
  <p>Độ ẩm: <span id="humidity">%HUMIDITY%</span> %</p>
  <button onclick="toggleLED()" id="ledBtn" class="off">Bật LED</button>
  <p>Góc Servo: <span id="servoVal">90</span>°</p>
  <input type="range" min="0" max="180" value="90" id="servoSlider" onchange="updateServo(this.value)">

<script>
function fetchData() {
  fetch('/temperature').then(r => r.text()).then(t => {
    document.getElementById("temperature").innerHTML = t;
  });
  fetch('/humidity').then(r => r.text()).then(h => {
    document.getElementById("humidity").innerHTML = h;
  });
  fetch('/ledstate').then(r => r.text()).then(state => {
    document.getElementById("ledstate").innerHTML = state;
    let btn = document.getElementById("ledBtn");
    if (state == "ON") {
      btn.innerText = "Tắt LED";
      btn.className = "on";
    } else {
      btn.innerText = "Bật LED";
      btn.className = "off";
    }
  });
}

function updateServo(val) {
  document.getElementById("servoVal").innerText = val;
  fetch("/servo?angle=" + val); // Gửi góc servo đến ESP8266
}

function toggleLED() {
  fetch("/toggleled").then(() => fetchData());
}

// Cập nhật mỗi 2 giây
setInterval(fetchData, 2000);
fetchData(); // lần đầu
</script>
</body>
</html>)rawliteral";

String processor(const String& var) {
  if (var == "TEMPERATURE") return String(t, 1);
  else if (var == "HUMIDITY") return String(h, 1);
  else if (var == "LEDSTATE") return ledState ? "ON" : "OFF";
  return String();
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // LOW = TẮT LED
  myServo.attach(SERVO_PIN);
  myServo.write(servoAngle); // Đặt servo về góc ban đầu

  WiFi.begin(ssid, password);
  Serial.print("Kết nối WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(t, 1));
  });

  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(h, 1));
  });

  server.on("/ledstate", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", ledState ? "ON" : "OFF");
  });

  server.on("/toggleled", HTTP_GET, [](AsyncWebServerRequest *request){
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW); // LOW = tat, HIGH = bat
    request->send(200, "text/plain", "OK");
  });

  server.on("/servo", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("angle")) {
      String angleStr = request->getParam("angle")->value();
      servoAngle = angleStr.toInt();
      servoAngle = constrain(servoAngle, 0, 180);
      myServo.write(servoAngle); // Cập nhật góc servo
      Serial.print("Góc servo: ");
      Serial.println(servoAngle);
      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "Missing angle");
    }
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
    Serial.printf("T: %.1f°C, H: %.1f%%\n", t, h);
  }
}
