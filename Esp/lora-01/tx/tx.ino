#include <SPI.h>
#include <LoRa.h>

#define BUTTON_PIN D3      // GPIO0
#define LORA_CS    D8
#define LORA_RST   -1
#define LORA_IRQ   D2

bool lastButtonState = HIGH;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed!");
    while (1);
  }
  Serial.println("LoRa Sender Ready");
}

void loop() {
  bool currentState = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH && currentState == LOW) {
    sendCommand("ON");
  } else if (lastButtonState == LOW && currentState == HIGH) {
    sendCommand("OFF");
  }

  lastButtonState = currentState;
  delay(50);  // debounce
}

void sendCommand(const String& cmd) {
  LoRa.beginPacket();
  LoRa.print(cmd);
  LoRa.endPacket();
  Serial.print("Sent: ");
  Serial.println(cmd);

  // Đợi phản hồi từ bên nhận
  unsigned long startTime = millis();
  bool ackReceived = false;
  while (millis() - startTime < 1000) {  // Timeout sau 1 giây
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
      String resp = "";
      while (LoRa.available()) {
        resp += (char)LoRa.read();
      }
      if (resp == "ACK") {
        Serial.println("ACK received ");
        ackReceived = true;
        break;
      }
    }
  }

  if (!ackReceived) {
    Serial.println(" No ACK received");
  }
}
