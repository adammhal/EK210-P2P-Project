#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x20, 16, 2);

const int RX_PIN = 6;
String receivedMessage = "";

const int MESSAGE_START_TIME = 1950;
const int MESSAGE_END_TIME = 1950;
const int BYTE_START_PULSE = 500;
const int BIT_DURATION = 250;
const int GAP_AFTER_BYTE_START = 250;
const int GAP_BETWEEN_LETTERS = 500;

void checkSerialForReset();
void waitForStartSignal();
byte receiveByte();

void setup() {
  Serial.begin(9600);
  pinMode(RX_PIN, INPUT);
  Wire.begin(8, 9);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Receiver Ready");
}

void checkSerialForReset() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c == 'r') {
      Serial.println("Reset command received. Restarting...");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Rebooting...");
      delay(500);
      ESP.restart();
    }
  }
}

void loop() {
  waitForStartSignal();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Receiving msg:");
  receivedMessage = "";

  while (true) {
    byte b = receiveByte();

    if (b == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Message Done!");
      lcd.setCursor(0, 1);
      if (receivedMessage.length() > 16) {
        lcd.print(receivedMessage.substring(0, 16));
      } else {
        lcd.print(receivedMessage);
      }
      Serial.println("\n--- Message Complete ---");
      Serial.println("Final Message: " + receivedMessage);
      delay(3000);
      break;
    } else {
      receivedMessage += (char)b;
      lcd.setCursor(0, 1);
      if (receivedMessage.length() > 16) {
        lcd.print(receivedMessage.substring(receivedMessage.length() - 16));
      } else {
        lcd.print(receivedMessage);
      }
    }
  }
}

void waitForStartSignal() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waiting for");
  lcd.setCursor(0, 1);
  lcd.print("Start Signal...");
  Serial.println("Waiting for START signal...");

  while (true) {
    while (digitalRead(RX_PIN) == HIGH) {
      checkSerialForReset();
    }

    unsigned long startTime = millis();
    while (digitalRead(RX_PIN) == LOW) {
      checkSerialForReset();
    }
    unsigned long duration = millis() - startTime;

    if (duration > MESSAGE_START_TIME) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Start Signal OK!");
      Serial.println("Start Signal Detected! Waiting for data...");
      return;
    } else {
      Serial.println("False signal, not long enough. Waiting again.");
    }
  }
}

byte receiveByte() {
  while (true) {
    unsigned long waitStartTime = millis();
    while (digitalRead(RX_PIN) == HIGH) {
      checkSerialForReset();
      if (millis() - waitStartTime > MESSAGE_END_TIME) {
        Serial.println("Timeout detected. Assuming END of message.");
        return 0;
      }
    }

    unsigned long blinkStartTime = millis();
    while (digitalRead(RX_PIN) == LOW) {
      checkSerialForReset();
    }
    unsigned long blinkDuration = millis() - blinkStartTime;

    if (blinkDuration > (BYTE_START_PULSE - 150) && blinkDuration < (BYTE_START_PULSE + 150)) {
      Serial.println("Byte-start blink detected. Receiving 8 bits...");
      delay(GAP_AFTER_BYTE_START);

      byte receivedData = 0;
      char debugBits[9];
      Serial.print("Receiving bits: ");
      for (int i = 7; i >= 0; i--) {
        delay(BIT_DURATION / 2);
        int bitState = digitalRead(RX_PIN);
        if (bitState == LOW) {
          receivedData |= (1 << i);
          Serial.print("1");
          debugBits[7 - i] = '1';
        } else {
          Serial.print("0");
          debugBits[7 - i] = '0';
        }
        delay(BIT_DURATION / 2);
      }
      debugBits[8] = '\0';

      Serial.println();
      Serial.print("Byte received: ");
      Serial.println((char)receivedData);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Byte: ");
      lcd.print((char)receivedData);

      lcd.setCursor(0, 1);
      lcd.print("Bits: ");
      lcd.print(debugBits);

      delay(GAP_BETWEEN_LETTERS);
      return receivedData;
    }

    Serial.println("Ignoring light pulse (not a byte-start).");
  }
}
