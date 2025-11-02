#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x20, 16, 2);

const int RX_PIN = 6;
const int LED_PIN = 10; 

unsigned long totalLcdLag = 0;

const int MESSAGE_START_TIME = 1950;
const int MESSAGE_END_TIME = 1950;
const int BYTE_START_PULSE = 500;
const int BIT_DURATION = 250;
const int GAP_AFTER_BYTE_START = 250;
const int GAP_BETWEEN_LETTERS = 500;

void setup() {
  Serial.begin(9600);
  pinMode(RX_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  Wire.begin(8, 9);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Test A (Slow Lag)");
}

int readRxPin() {
  int state = digitalRead(RX_PIN);
  digitalWrite(LED_PIN, (state == LOW) ? HIGH : LOW);
  return state;
}

void loop() {
  waitForStartSignal();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Receiving...");
  
  totalLcdLag = 0;
  
  while (true) {
    byte b = receiveByte();
    if (b == 0) {
      break; 
    }
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lag Time (ms):");
  lcd.setCursor(0, 1);
  lcd.print(totalLcdLag);
  Serial.print("Total LCD Lag (Slow): ");
  Serial.println(totalLcdLag);
  
  delay(3000);
}

void waitForStartSignal() {
  lcd.setCursor(0, 1);
  lcd.print("Waiting...");
  while (true) {
    while (readRxPin() == HIGH) {}
    unsigned long startTime = millis();
    while (readRxPin() == LOW) {}
    unsigned long duration = millis() - startTime;
    if (duration > MESSAGE_START_TIME) {
      return;
    }
  }
}

byte receiveByte() {
  while (true) {
    unsigned long waitStartTime = millis();
    while (readRxPin() == HIGH) {
      if (millis() - waitStartTime > MESSAGE_END_TIME) {
        return 0;
      }
    }
    unsigned long blinkStartTime = millis();
    while (readRxPin() == LOW) {}
    unsigned long blinkDuration = millis() - blinkStartTime;

    if (blinkDuration > (BYTE_START_PULSE - 150) && blinkDuration < (BYTE_START_PULSE + 150)) {
      delay(GAP_AFTER_BYTE_START);
      byte receivedData = 0;
      char debugBits[9]; 

      for (int i = 7; i >= 0; i--) {
        delay(BIT_DURATION / 2);
        int bitState = readRxPin();
        
        unsigned long lcdStartTime = millis();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Receiving Byte");
        lcd.setCursor(0, 1);
        lcd.print("Bit "); lcd.print(i); lcd.print(": ");

        if (bitState == LOW) {
          receivedData |= (1 << i);
          debugBits[7 - i] = '1';
          lcd.print("1");
        } else {
          debugBits[7 - i] = '0';
          lcd.print("0");
        }
        totalLcdLag += (millis() - lcdStartTime);
        
        delay(BIT_DURATION / 2);
      }
      debugBits[8] = '\0';
      delay(GAP_BETWEEN_LETTERS);
      return receivedData;
    }
  }
}
