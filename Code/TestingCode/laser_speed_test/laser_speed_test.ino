#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); 

const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {'A','D','G'},
  {'J','M','P'},
  {'S','V','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {36, 41, 40, 38}; 
byte colPins[COLS] = {37, 35, 39}; 

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int LASER_PIN = 7; 
String message = "hello";

const int MESSAGE_START_PULSE = 2000;
const int MESSAGE_END_PAUSE   = 2000;
const int BYTE_START_PULSE    = 500;
const int BIT_DURATION        = 250;
const int GAP_AFTER_BYTE_START = 250;
const int GAP_BETWEEN_LETTERS = 500; 

void updateTopLCD(String s) {
  lcd.setCursor(0, 0);
  lcd.print(s);
  for (int i = s.length(); i < 16; i++) {
    lcd.print(" ");
  }
}

void setup() {
  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, LOW); 

  Wire.begin(8, 9); 
  lcd.init();
  
  lcd.backlight();
  updateTopLCD("Speed Test Ready");
  lcd.setCursor(0, 1);
  lcd.print("Press any key...");
  lcd.noBlink(); 
}

void loop(){
  char key = keypad.getKey(); 
  
  if (key) {
    updateTopLCD("Sending Test...");
    lcd.setCursor(0, 1);
    lcd.print(message);
    
    transmitMessage();
    
    updateTopLCD("Test Complete.");
    lcd.setCursor(0, 1);
    lcd.print("Press any key...");
  }
}

void transmitMessage() {
  digitalWrite(LASER_PIN, HIGH);
  delay(MESSAGE_START_PULSE);
  digitalWrite(LASER_PIN, LOW);
  delay(GAP_BETWEEN_LETTERS); 

  for (int i = 0; i < message.length(); i++) {
    sendByte(message.charAt(i));
  }

  digitalWrite(LASER_PIN, LOW);
  delay(MESSAGE_END_PAUSE);
}

void sendByte(char data) {
  digitalWrite(LASER_PIN, HIGH);
  delay(BYTE_START_PULSE);

  digitalWrite(LASER_PIN, LOW);
  delay(GAP_AFTER_BYTE_START);

  for (int i = 7; i >= 0; i--) {
    bool bit_is_one = (data >> i) & 1;
    if (bit_is_one) {
      digitalWrite(LASER_PIN, HIGH); 
    } else {
      digitalWrite(LASER_PIN, LOW);  
    }
    delay(BIT_DURATION);
  }

  digitalWrite(LASER_PIN, LOW);
  delay(GAP_BETWEEN_LETTERS);
}
