#include <DHT.h>
#include <SPI.h>
#include <LoRa.h>

// Cấu hình chân DHT11 - Sửa thành chân 3
#define DHTPIN 3       // Chân DATA nối với chân 3 Arduino
#define DHTTYPE DHT11  // Loại cảm biến DHT11

DHT dht(DHTPIN, DHTTYPE); // Khởi tạo cảm biến

float temp = 0;
float humidity = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("LoRa Temperature Sender (DHT11 on PIN 3)");

  // Khởi tạo LoRa
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed. Check wiring!");
    while (1);
  }
  Serial.println("LoRa initialized!");

  // Khởi tạo cảm biến DHT11
  dht.begin();
  Serial.println("DHT11 ready!");
}

void loop() {
  // Đọc nhiệt độ và độ ẩm
  temp = dht.readTemperature();    // Nhiệt độ (°C)
  humidity = dht.readHumidity();   // Độ ẩm (%)

  // Kiểm tra nếu đọc lỗi
  if (isnan(temp) || isnan(humidity)) {
    Serial.println("Failed to read DHT sensor!");
    delay(2000);
    return;
  }

  // In ra Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print("°C  |  Humidity: ");
  Serial.print(humidity);
  Serial.println("%");

  // Gửi dữ liệu qua LoRa (định dạng "nhiệt_độ độ_ẩm")
  String loraData = String(temp, 1) + " " + String(humidity, 1); // Làm tròn 1 số thập phân
  Serial.print("Sending LoRa packet: ");
  Serial.println(loraData);

  LoRa.beginPacket();
  LoRa.print(loraData);
  LoRa.endPacket();

  delay(2000); // Gửi mỗi 2 giây
}