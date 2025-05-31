#include <LiquidCrystal_I2C.h>

#define LORA_RX 16
#define LORA_TX 17
#define M0 25
#define M1 26
#define LED 2

LiquidCrystal_I2C lcd(0x27, 16, 2); // Địa chỉ I2C 0x27, LCD 16x2
HardwareSerial mySerial(1);

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600, SERIAL_8N1, LORA_RX, LORA_TX);
  
  // Khởi tạo LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("DHT11 LORA");
  lcd.setCursor(0, 1);
  lcd.print("LONG AUTOMATION !");
  
  // Cấu hình chế độ LoRa
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  digitalWrite(M0, LOW);  // Chế độ Normal
  digitalWrite(M1, LOW);
  
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  
  Serial.println("LoRa Communication Ready");
  delay(2000);
}

void loop() {
  static String lastDisplay = ""; // Lưu trạng thái hiển thị cuối
  String inputReceive = "";
  
  // Kiểm tra dữ liệu LoRa với timeout 2s
  unsigned long startTime = millis();
  while (mySerial.available() == 0 && millis() - startTime < 2000) {
    delay(10);
  }
  
  if (mySerial.available() > 0) {
    inputReceive = mySerial.readStringUntil('\n');
    inputReceive.trim(); // Loại bỏ ký tự thừa
    Serial.println("Received: " + inputReceive);
    digitalWrite(LED, HIGH); // Báo hiệu có dữ liệu
    delay(100);
    digitalWrite(LED, LOW);
    
    // Chỉ cập nhật LCD nếu dữ liệu thay đổi
    if (inputReceive != lastDisplay) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temp and Humi:");
      lcd.setCursor(0, 1);
      lcd.print(inputReceive);
      lastDisplay = inputReceive;
    }
  } 
  else {
    Serial.println("Waiting for LoRa data...");
    // Hiển thị trạng thái chờ nếu không có dữ liệu
    if (lastDisplay != "No Data") {
      lcd.setCursor(0, 1);
      lcd.print("No Signal     ");
      lastDisplay = "No Data";
    }
  }
  
  delay(1000); // Giảm tốc độ đọc để tránh nhấp nháy LCD
}