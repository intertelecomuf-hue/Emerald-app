/**
 * touch_test.ino — тест XPT2046
 */
#include <SPI.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
#define CS_PIN 21
#define IRQ_PIN 36
void setup() {
    Serial.begin(115200); delay(1000);
    Serial.println("\n=== TOUCH TEST ===");
    pinMode(CS_PIN, OUTPUT); digitalWrite(CS_PIN, HIGH);
    pinMode(IRQ_PIN, INPUT_PULLUP);
    SPI.begin(14, 12, 13, 21);
    SPI.setFrequency(2000000);
    Serial.println("SPI ready. Touch screen...");
}
void loop() {
    if (digitalRead(IRQ_PIN) == LOW) {
        digitalWrite(CS_PIN, LOW);
        delayMicroseconds(1);
        SPI.transfer(0x90);
        int x = SPI.transfer(0x00) << 4;
        x |= SPI.transfer(0x00) >> 4;
        SPI.transfer(0xD0);
        int y = SPI.transfer(0x00) << 4;
        y |= SPI.transfer(0x00) >> 4;
        digitalWrite(CS_PIN, HIGH);
        if (x > 50 && x < 4045 && y > 50 && y < 4045)
            Serial.printf("TOUCH: raw(%d,%d)\n", x, y);
    }
    delay(50);
}
