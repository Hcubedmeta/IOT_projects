#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Địa chỉ I2C 0x27, LCD 16x2

void setup() {
  Serial.begin(9600);
  while (!Serial); // Chờ Serial ready
  
  // Khởi tạo LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Initializing...");
  
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed!");
    lcd.clear();
    lcd.print("LoRa Failed!");
    while (1);
  }
  
  lcd.clear();
  lcd.print("LoRa Ready!");
  delay(1000);
}

void loop() {
  if (LoRa.parsePacket()) {
    String str = "";
    
    // Đọc toàn bộ gói tin
    while (LoRa.available()) {
      str += (char)LoRa.read();
    }
    
    // Tách nhiệt độ và độ ẩm
    int spaceIndex = str.indexOf(' '); // Tìm vị trí khoảng trắng
    if (spaceIndex != -1) {
      String tempc = str.substring(0, spaceIndex);
      String humidity = str.substring(spaceIndex + 1);
      
      // Hiển thị lên Serial
      Serial.print("Temp: ");
      Serial.print(tempc);
      Serial.print("°C, Humidity: ");
      Serial.print(humidity);
      Serial.print("%, RSSI: ");
      Serial.println(LoRa.packetRssi());
      
      // Hiển thị lên LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.print(tempc);
      lcd.print("C");
      
      lcd.setCursor(0, 1);
      lcd.print("Humidity: ");
      lcd.print(humidity);
      lcd.print("%");
    } else {
      Serial.println("Invalid data format!");
    }
  }
  delay(100);
}