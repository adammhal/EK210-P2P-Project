#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x20, 16, 2);

const int RX_PIN = 6;
const int LED_PIN = 10; 
String receivedMessage = "";
int messageRow = 0;

const int MESSAGE_START_TIME = 1950; 
const int CALIBRATION_TIME = 2000;
const int MESSAGE_END_TIME = 1950;
const int BYTE_START_PULSE = 500;
const int BIT_DURATION = 250;
const int GAP_AFTER_BYTE_START = 250;
const int GAP_BETWEEN_LETTERS = 500;

void checkSerialForReset();
void waitForStartSignal();
byte receiveByte();
void calibrateReceiver();
int readRxPin();

void setup() {
  Serial.begin(9600);
  pinMode(RX_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  Wire.begin(8, 9);
  lcd.init();
  lcd.backlight();
  
  calibrateReceiver();
}

int readRxPin() {
  int state = digitalRead(RX_PIN);
  if (state == LOW) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  return state;
}

void calibrateReceiver() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibration Mode");
  lcd.setCursor(0, 1);
  lcd.print("Point laser now...");

  while (readRxPin() == HIGH) {
    checkSerialForReset();
  }

  unsigned long startTime = millis();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Laser Detected!");
  lcd.setCursor(0, 1);
  lcd.print("Hold for 2 sec...");
  bool holdOK = false;

  while (readRxPin() == LOW) {
    checkSerialForReset();
    
    if (!holdOK && (millis() - startTime >= CALIBRATION_TIME)) {
      holdOK = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hold OK!");
      lcd.setCursor(0, 1);
      lcd.print("Release laser...");
    }
  }

  if (holdOK) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Calibration OK!");
    lcd.setCursor(0, 1);
    lcd.print("Receiver Ready.");
    delay(2000);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hold Failed!");
    lcd.setCursor(0, 1);
    lcd.print("Hold longer.");
    delay(2000);
    calibrateReceiver();
  }
}


void checkSerialForReset() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c == 'r') {
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
  messageRow = 0;

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
      delay(3000);
      break;
    } else {
      receivedMessage += (char)b;
      
      if (messageRow == 0) {
        int startPos = 16 - receivedMessage.length();
        
        if (startPos < 14) {
          messageRow = 1; 
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Receiving (L2):");
          lcd.setCursor(0, 1);
          lcd.print(receivedMessage);
        } else {
          lcd.setCursor(startPos, 0);
          lcd.print(receivedMessage);
        }
      } else {
        lcd.setCursor(0, 1);
        if (receivedMessage.length() > 16) {
          lcd.print(receivedMessage.substring(receivedMessage.length() - 16));
        } else {
          lcd.print(receivedMessage);
        }
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

  while (true) {
    while (readRxPin() == HIGH) {
      checkSerialForReset();
    }

    unsigned long startTime = millis();
    while (readRxPin() == LOW) {
      checkSerialForReset();
    }
    unsigned long duration = millis() - startTime;

    if (duration > MESSAGE_START_TIME) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Start Signal OK!");
      return;
    }
  }
}

byte receiveByte() {
  while (true) {
    unsigned long waitStartTime = millis();
    while (readRxPin() == HIGH) {
      checkSerialForReset();
      if (millis() - waitStartTime > MESSAGE_END_TIME) {
        return 0;
      }
    }

    unsigned long blinkStartTime = millis();
    while (readRxPin() == LOW) {
      checkSerialForReset();
    }
    unsigned long blinkDuration = millis() - blinkStartTime;

    if (blinkDuration > (BYTE_START_PULSE - 150) && blinkDuration < (BYTE_START_PULSE + 150)) {
      delay(GAP_AFTER_BYTE_START);

      byte receivedData = 0;
      for (int i = 7; i >= 0; i--) {
        delay(BIT_DURATION / 2);
        int bitState = readRxPin();
        if (bitState == LOW) {
          receivedData |= (1 << i);
        }
        delay(BIT_DURATION / 2);
      }

      delay(GAP_BETWEEN_LETTERS);
      return receivedData;
    }
  }
}