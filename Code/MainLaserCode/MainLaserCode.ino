#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); 

const byte ROWS = 4;
const byte COLS = 3;
const int servoPin = 4; 

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

const int MESSAGE_START_PULSE = 2000;
const int MESSAGE_END_PAUSE   = 2000;
const int BYTE_START_PULSE    = 500;
const int BIT_DURATION        = 250;
const int GAP_AFTER_BYTE_START = 250;
const int GAP_BETWEEN_LETTERS = 500; 

float angleDegree = 1500;
float currentAngle = angleDegree;

String messageBuffer = ""; 
char currentKey = 0;
int currentKeyPressCount = 0;
unsigned long lastKeyPressTime = 0;
const int T9_TIMEOUT = 1000; 

unsigned long nineKeyPressTime = 0;
const int NINE_KEY_TIMEOUT = 500;
int ninePressCount = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(servoPin, OUTPUT);
  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, LOW); 

  Wire.begin(8, 9); 
  lcd.init();
  
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Enter Message:");
  lcd.setCursor(0, 1);
  lcd.blink(); 
  
  Serial.println("Transmitter Ready.");
}

void loop(){
  if (ninePressCount > 0 && (millis() - nineKeyPressTime > NINE_KEY_TIMEOUT)) {
    handlePendingNineAction();
  }

  if (keypad.isPressed('*')) {
    handlePendingNineAction();
    finalizeCurrentLetter();
    Serial.println("Servo Left (Hold)");
    if (currentAngle < 1900) {
      currentAngle = calculateStepLeft(currentAngle);
      moveStepLeft(currentAngle);
    }
    delay(50); 
  }
  
  if (keypad.isPressed('#')) {
    handlePendingNineAction();
    finalizeCurrentLetter();
    Serial.println("Servo Right (Hold)");
    if (currentAngle > 1000) {
      currentAngle = calculateStepRight(currentAngle);
      moveStepRight(currentAngle);
    }
    delay(50);
  }

  if (currentKey != 0 && (millis() - lastKeyPressTime > T9_TIMEOUT)) {
    finalizeCurrentLetter();
  }

  char key = keypad.getKey(); 
  
  if (key) {
    Serial.println(key);
    
    if (key == '0') {
      handlePendingNineAction();
      finalizeCurrentLetter();
      Serial.println("Zeroing Servo");
      for (int i = 0; i < 5; i++){
        zeroInitialAngle();
      }
      currentAngle = 1500;
    }
    else if (key == '*' || key == '#') {
      handlePendingNineAction();
      finalizeCurrentLetter();
    }
    
    else if (key == '9') {
      if (currentKey != 0) {
        lcd.setCursor(messageBuffer.length(), 1);
        lcd.print(" "); 
        lcd.setCursor(messageBuffer.length(), 1);
        lcd.blink();
        currentKey = 0;
        currentKeyPressCount = 0;
        lastKeyPressTime = 0;
      }
      
      ninePressCount++;
      nineKeyPressTime = millis(); 

      if (ninePressCount == 1) {
        Serial.println("9-Key: Press 1 (Backspace pending)");
        lcd.setCursor(0, 0); lcd.print("Backspace?");
      } else if (ninePressCount == 2) {
        Serial.println("9-Key: Press 2 (Space pending)");
        lcd.setCursor(0, 0); lcd.print("Add Space?");
      } else if (ninePressCount == 3) {
        Serial.println("9-Key: Press 3 (SUBMIT)");
        lcd.setCursor(0, 0); lcd.print("Sending...");
        
        transmitMessage(); 
        messageBuffer = ""; 
        ninePressCount = 0; 
        
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Message Sent!");
        delay(1500);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter Message:");
        lcd.setCursor(0, 1);
        lcd.blink();
      }
    }
    
    else { 
      handlePendingNineAction();
      
      if (key != currentKey) {
        finalizeCurrentLetter();
        currentKey = key;
        currentKeyPressCount = 0;
      } 
      else {
        currentKeyPressCount++;
      }

      if (currentKey == 'V' || currentKey == 'S') { 
        currentKeyPressCount = currentKeyPressCount % 4;
      } else {
        currentKeyPressCount = currentKeyPressCount % 3;
      }
      
      lastKeyPressTime = millis(); 
      
      char previewLetter = (char)convertToASCII(currentKey, currentKeyPressCount);
      
      int cursorColumn = messageBuffer.length();
      lcd.setCursor(cursorColumn, 1);
      lcd.print(previewLetter);
      lcd.setCursor(cursorColumn, 1); 
    }
  }
}

void handlePendingNineAction() {
  if (ninePressCount == 0) return;

  if (ninePressCount == 1) {
    Serial.println("9-Key: Executing Backspace");
    doBackspace();
  } else if (ninePressCount >= 2) {
    Serial.println("9-Key: Executing Add Space");
    addSpace();
  }
  ninePressCount = 0;
  
  lcd.setCursor(0, 0); 
  lcd.print("Enter Message:   "); 
  
  lcd.setCursor(messageBuffer.length(), 1);
  lcd.blink();
}

void addSpace() {
  messageBuffer += " ";
  int cursorColumn = messageBuffer.length();
  lcd.setCursor(cursorColumn - 1, 1);
  lcd.print(" ");
  lcd.setCursor(cursorColumn, 1);
  lcd.blink();
  Serial.println("Buffer: " + messageBuffer);
}

void doBackspace() {
  if (messageBuffer.length() > 0) {
    messageBuffer = messageBuffer.substring(0, messageBuffer.length() - 1);
    
    int cursorColumn = messageBuffer.length();
    lcd.setCursor(cursorColumn, 1);
    lcd.print(" "); 
    lcd.setCursor(cursorColumn, 1);
    lcd.blink();
    Serial.println("Buffer: " + messageBuffer);
  }
}

void finalizeCurrentLetter() {
  if (currentKey == 0) {
    return;
  }
  
  char finalLetter = (char)convertToASCII(currentKey, currentKeyPressCount);
  messageBuffer += finalLetter;
  
  int cursorColumn = messageBuffer.length() - 1;
  lcd.setCursor(cursorColumn, 1);
  lcd.print(finalLetter);
  
  lcd.setCursor(messageBuffer.length(), 1);
  
  Serial.print("Letter locked: "); Serial.println(finalLetter);
  Serial.print("Buffer: "); Serial.println(messageBuffer);

  currentKey = 0;
  currentKeyPressCount = 0;
}

void transmitMessage() {
  if (messageBuffer.length() == 0) {
    Serial.println("Buffer empty, nothing to send.");
    lcd.clear();
    lcd.print("Buffer Empty!");
    delay(1000);
    lcd.setCursor(0, 0);
    lcd.print("Enter Message:");
    lcd.setCursor(0, 1);
    lcd.blink();
    return;
  }
  
  Serial.println("Sending message: " + messageBuffer);
  lcd.noBlink();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sending...");
  lcd.setCursor(0, 1);
  lcd.print(messageBuffer);

  Serial.println("Sending START signal...");
  digitalWrite(LASER_PIN, HIGH);
  delay(MESSAGE_START_PULSE);
  digitalWrite(LASER_PIN, LOW);
  delay(GAP_BETWEEN_LETTERS); 

  for (int i = 0; i < messageBuffer.length(); i++) {
    sendByte(messageBuffer.charAt(i));
  }

  Serial.println("Sending END signal...");
  digitalWrite(LASER_PIN, LOW);
  delay(MESSAGE_END_PAUSE);
  
  Serial.println("--- Message complete ---");
}

void sendByte(char data) {
  Serial.print("Sending '");
  Serial.print(data);
  Serial.print("' (");
  Serial.print(data, BIN);
  Serial.println("):");

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
  Serial.println("Byte complete.");
  delay(GAP_BETWEEN_LETTERS);
}

int convertToASCII(char loggedMessage, int repeatedStrokes) {
  int asciiValue = (int)loggedMessage + repeatedStrokes;
  Serial.print("Converted to ASCII: "); Serial.println(asciiValue);
  return asciiValue;
}

void zeroInitialAngle(){
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(1500);
  digitalWrite(servoPin, LOW);
  delayMicroseconds(1000);
}

float calculateStepLeft(float currentAngle){
  float toMoveHIGHMicroseconds = currentAngle + 11.11111111111;
  return toMoveHIGHMicroseconds;
}

float calculateStepRight(float currentAngle){
  float toMoveLOWMicroseconds = currentAngle - 11.11111111111;
  return toMoveLOWMicroseconds;
}

void moveStepLeft(float currentAngle){
  Serial.println("Moving Positive Direction");
  digitalWrite(servoPin, HIGH);
  float toMoveHIGHMicroseconds = currentAngle;
  delayMicroseconds(toMoveHIGHMicroseconds);
  digitalWrite(servoPin, LOW);
  delayMicroseconds(2500 - toMoveHIGHMicroseconds);
}

void moveStepRight(float currentAngle){
  Serial.println("Moving Negative Direction");
  float toMoveHIGHMicroseconds = currentAngle;
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(toMoveHIGHMicroseconds);
  digitalWrite(servoPin, LOW);
  delayMicroseconds(2500 - toMoveHIGHMicroseconds);
}