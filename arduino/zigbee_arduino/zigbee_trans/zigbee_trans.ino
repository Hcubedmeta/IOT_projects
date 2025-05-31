#include <DHT.h>
#include <SoftwareSerial.h>

#define ZIGBEE_RX 8   // Arduino nhận từ Zigbee TX
#define ZIGBEE_TX 9   // Arduino gửi đến Zigbee RX
#define LED 13        // LED tích hợp trên Arduino Uno

#define DHTPIN 4      // Chân cảm biến DHT11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Khởi tạo phần mềm UART cho Zigbee
SoftwareSerial zigbeeSerial(ZIGBEE_RX, ZIGBEE_TX); // RX, TX

void setup() {
  Serial.begin(9600);            // UART chính (debug)
  zigbeeSerial.begin(9600);      // UART phần mềm cho Zigbee

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  dht.begin();

  Serial.println("System Ready - DHT11 + Zigbee CC2530");
}

void loop() {
  static unsigned long lastSendTime = 0;
  const unsigned long interval = 2000; // Gửi mỗi 2 giây

  if (millis() - lastSendTime >= interval) {
    lastSendTime = millis();

// Sau khi đọc dữ liệu DHT11
float temperature = dht.readTemperature();
float humidity = dht.readHumidity();

if (!isnan(temperature) && !isnan(humidity)) {
  String data = "NHIET DO " + String(temperature, 1) + "C\n";
  data += "DO AM " + String(humidity, 1) + "%";

  Serial.print("Sending: ");
  Serial.println(data);

  zigbeeSerial.println(data);  // Gửi qua Zigbee
}
else {
      Serial.println("Failed to read from DHT sensor!");
      digitalWrite(LED, HIGH);
      delay(100);
      digitalWrite(LED, LOW);
    }
  }

  delay(100); // Nhẹ CPU
}
