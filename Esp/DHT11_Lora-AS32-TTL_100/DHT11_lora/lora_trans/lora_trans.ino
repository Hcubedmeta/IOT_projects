#include <DHT.h>
#define LORA_RX 16
#define LORA_TX 17
#define M0 25
#define M1 26
#define LED 2

HardwareSerial mySerial(1);

#define DHTPIN 4     // Chân kết nối DHT11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600, SERIAL_8N1, LORA_RX, LORA_TX);

  // Cấu hình chế độ LoRa
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  digitalWrite(M0, LOW);  // Normal mode
  digitalWrite(M1, LOW);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  dht.begin();
  Serial.println("System Ready - DHT11 + LoRa");
}

void loop() {
  static unsigned long lastSendTime = 0;
  const unsigned long interval = 2000; // Gửi mỗi 2 giây

  if (millis() - lastSendTime >= interval) {
    lastSendTime = millis();
    
    // Đọc dữ liệu từ cảm biến
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Kiểm tra dữ liệu hợp lệ
    if (!isnan(temperature) && !isnan(humidity)) {
      String data = String(temperature) + "," + String(humidity);
      
      // Gửi qua Serial Monitor để debug
      Serial.print("Sending: ");
      Serial.println(data);
      
      // Gửi qua LoRa
      mySerial.println(data);
      
      // Báo hiệu gửi thành công bằng LED
      digitalWrite(LED, HIGH);
      delay(50);
      digitalWrite(LED, LOW);
    } else {
      Serial.println("Failed to read from DHT sensor!");
      digitalWrite(LED, HIGH); // Báo lỗi bằng LED sáng liên tục
      delay(100);
      digitalWrite(LED, LOW);
    }
  }
  
  delay(100); // Giảm CPU usage
}