#include <Key.h>
#include <Keypad.h>


// Define number of rows and columns
const byte ROWS = 4;
const byte COLS = 3;
const int servoPin = 6;

const int LASER_PIN = 4;
String messageToSend;

const int MESSAGE_START_PULSE = 2000;
const int MESSAGE_END_PAUSE = 2000;
const int BYTE_START_PULSE = 500;
const int BIT_DURATION = 250;
const int GAP_AFTER_BYTE_START = 100;
const int GAP_BETWEEN_LETTERS = 250;

// Define the key layout
char keys[ROWS][COLS] = {
  {'A','D','G'},
  {'J','M','P'},
  {'S','V','9'},
  {'*','0','#'}
};
//PID 3845 has Column 0 in Pin 7, Column 1 in Pin 9, Column 2 in Pin 5
//PID 3845 has Row 0 in Pin 8, Row 1 in Pin 3, Row 2 in Pin 4, and Row 4 in Pin 6
// Connect keypad ROW0, ROW1, ROW2, ROW3 to Arduino pins:
byte rowPins[ROWS] = {12, 7, 8, 10};

// Connect keypad COL0, COL1, COL2 to Arduino pins:
byte colPins[COLS] = {11, 13, 9};

// Create keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  int setupValue = 0;
  Serial.begin(9600);
  Serial.println("Adafruit 4x3 Keypad Test");
  pinMode(servoPin, OUTPUT);
}



float angleDegree = 1500;
float currentAngle = angleDegree;
char loggedKeyMessage;

void loop(){
  
  char key = keypad.getKey();  // Read key press
  
if (key) {
  Serial.println(key);
  if (key == '0')
  {
  for (int i = 0; i < 5; i++){
    Serial.println("Hello");
    zeroInitialAngle();
    
  }
  currentAngle = 1500;
  delay(2500);
  Serial.println(currentAngle);
  }

  else if (key == '*') {
  Serial.println(key);
    if (currentAngle < 1900) {
      float moveToAngle = calculateStepLeft(currentAngle);
      for (int i = 0; i < 5; i++){
        moveStepLeft(moveToAngle);
      //Serial.println(currentAngle);
      }
      Serial.println(currentAngle);
      currentAngle = moveToAngle;
      Serial.println(moveToAngle);
      Serial.println(currentAngle);
    }
    
    key = keypad.getKey();
    Serial.println(key);
  }
  

  else if (key == '#') {
    Serial.println(key);
    if (currentAngle > 1000) {
      float moveToAngle = calculateStepRight(currentAngle);
      for (int i = 0; i < 5; i++){
        moveStepRight(moveToAngle);
      //Serial.println(currentAngle);
      }
      Serial.println(currentAngle);
      currentAngle = moveToAngle;
      Serial.println(moveToAngle);
      Serial.println(currentAngle);
      
    }
    key = keypad.getKey();
}
  else {
    loggedKeyMessage = key;
    int countedKeystrokes;
    countedKeystrokes = sameKeyInputMessage(loggedKeyMessage);
    Serial.println(countedKeystrokes);

    
    int asciiValue;
    String binaryValue;
    asciiValue = convertToASCII(loggedKeyMessage, countedKeystrokes);
    binaryValue = convertToBinary(asciiValue);
    messageToSend = binaryValue;

    Serial.println(binaryValue);
    delay(1000);

    //Priting the message

    Serial.println("Sending message: " + messageToSend);
    digitalWrite(LASER_PIN, HIGH);
    delay(MESSAGE_START_PULSE);
    digitalWrite(LASER_PIN, LOW);
    delay(GAP_BETWEEN_LETTERS);

    for (int i = 0; i < messageToSend.length(); i++) {
        sendByte(messageToSend.charAt(i));
    }

    digitalWrite(LASER_PIN, LOW);
    delay(MESSAGE_END_PAUSE);

    Serial.println("Message Sending complete.");
    delay(2000);
  }
}
}
  // delay(500);
int convertToASCII(char loggedMessage, int repeatedStrokes) {
  int asciiValue = (int)loggedMessage + repeatedStrokes;
  Serial.println(asciiValue);
  return asciiValue;
}

String convertToBinary(int AsciiValue) {
  Serial.print("Binary: ");
  String emptyCharacterArray;
   for (int i = 7; i >= 0; i--) {
      int bitVal = (AsciiValue >> i) & 1;
      Serial.print(bitVal);
      emptyCharacterArray = emptyCharacterArray + bitVal;
      Serial.println(emptyCharacterArray);
   }
   
   Serial.println(emptyCharacterArray);
  char completedCharArray;
  //for (int d = 0; d < strlen(emptyCharacterArray) - 1; d++) {
    //  completedCharArray = completedCharArray + emptyCharacterArray[d];
  //}
  //Serial.println(completedCharArray);
  //Serial.println();
  return emptyCharacterArray;
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

int sameKeyInputMessage(char loggedKey){
  int count = 0;
  char key;
  char keyforloop;
  for (int i = 0; i < 2; i++){
    key = keypad.getKey();
    keyforloop = key;
    while (!key) {
      key = keypad.getKey();
      if (!key) {
        continue;
      }
      else if (key == '9'){
        keyforloop = key;
        break;
      }
      else if (key) {
        if (key == loggedKey) {
          count ++;
          keyforloop = key;
          break;
        }
      }
      else {
        break;
      }
    }
    if (keyforloop == '9') {
      break;
    }
  }
  Serial.println(count);
  delay(1000);
  return count;
}


void zeroInitialAngle(){ //calculates the initial angle of 90 degrees to point vertically
    Serial.println("Setup");
    digitalWrite(servoPin, HIGH);
    delayMicroseconds(1500);
    digitalWrite(servoPin, LOW);
    delayMicroseconds(1000);
    Serial.println("Zeroing Complete");
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
  currentAngle = toMoveHIGHMicroseconds;
}

void moveStepRight(float currentAngle){
  //Explore this once done with the moveStepPositive
  Serial.println("Moving Negative Direction");
  float toMoveHIGHMicroseconds = currentAngle;
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(toMoveHIGHMicroseconds);
  digitalWrite(servoPin, LOW);
  delayMicroseconds(2500 - toMoveHIGHMicroseconds);
}


