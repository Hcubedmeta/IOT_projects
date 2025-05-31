#include <SPI.h>
#include <LoRa.h>
#include <Servo.h>

#define SERVO_PIN D4       // GPIO2
#define LORA_CS   D8
#define LORA_RST  -1
#define LORA_IRQ  D2

Servo myServo;

void setup() {
  Serial.begin(9600);
  myServo.attach(SERVO_PIN);
  myServo.write(0);  // Vị trí ban đầu

  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed!");
    while (1);
  }
  Serial.println("LoRa Receiver Ready");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String cmd = "";
    while (LoRa.available()) {
      cmd += (char)LoRa.read();
    }

    Serial.print("Received: ");
    Serial.println(cmd);

    if (cmd == "ON") {
      myServo.write(90);
    } else if (cmd == "OFF") {
      myServo.write(0);
    }

    // Gửi lại ACK
    delay(100);  // Chờ để tránh xung đột tần số
    LoRa.beginPacket();
    LoRa.print("ACK");
    LoRa.endPacket();
  }
}
