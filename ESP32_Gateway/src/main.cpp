#include <Arduino.h>

#define RXD1 44
#define TXD1 43

void setup() 
{
    // Serial này dùng để in lên màn hình máy tính (USB)
    Serial.begin(115200); 
    
    // Serial1 dùng để đọc từ STM32
    Serial1.begin(115200, SERIAL_8N1, RXD1, TXD1);
    
    Serial.println("Đang đợi dữ liệu từ STM32...");
}

void loop() {
    if (Serial1.available()) {
    // Đọc từng ký tự từ STM32 và in thẳng lên màn hình
    String data = Serial1.readStringUntil('\n');
    Serial.print("Dữ liệu nhận được: ");
    Serial.println(data);
  }
}