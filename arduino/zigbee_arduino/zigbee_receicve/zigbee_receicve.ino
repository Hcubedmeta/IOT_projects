#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

#define ZIGBEE_RX 8   // Arduino RX <- Zigbee TX
#define ZIGBEE_TX 9   // Arduino TX -> Zigbee RX
#define LED 13        // LED báo hiệu

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD 16x2 I2C địa chỉ 0x27
SoftwareSerial zigbeeSerial(ZIGBEE_RX, ZIGBEE_TX); // RX, TX

void setup() {
  Serial.begin(9600);            // Serial debug
  zigbeeSerial.begin(9600);      // Zigbee UART

  // Khởi tạo LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("DHT11 ZIGBEE");
  lcd.setCursor(0, 1);
  lcd.print("LONG AUTOMATION!");

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  Serial.println("Zigbee Receiver Ready");
  delay(2000);
}

void loop() {
  static String lastDisplay = "";
  String inputReceive = "";

  // Chờ dữ liệu đến trong 2s
  unsigned long startTime = millis();
  while (zigbeeSerial.available() == 0 && millis() - startTime < 2000) {
    delay(10);
  }

if (zigbeeSerial.available() > 0) {
  String line1 = zigbeeSerial.readStringUntil('\n');
  String line2 = zigbeeSerial.readStringUntil('\n');
  
  line1.trim();
  line2.trim();

  Serial.println("Received:");
  Serial.println(line1);
  Serial.println(line2);

  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}
 else {
    Serial.println("Waiting for Zigbee data...");
    if (lastDisplay != "No Data") {
      lcd.setCursor(0, 1);
      lcd.print("No Signal     ");
      lastDisplay = "No Data";
    }
  }

  delay(1000); // Tránh nhấp nháy LCD
}
