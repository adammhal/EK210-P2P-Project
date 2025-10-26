const int LASER_PIN = 7;
String message = "hello world";

const int MESSAGE_START_PULSE = 2000;
const int MESSAGE_END_PAUSE = 2000;
const int BYTE_START_PULSE = 500;
const int BIT_DURATION = 250;
const int GAP_AFTER_BYTE_START = 250;
const int GAP_BETWEEN_LETTERS = 500;

void setup() {
  Serial.begin(9600);
  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, LOW);
}

void loop() {
  Serial.println("Sending message: " + message);
  digitalWrite(LASER_PIN, HIGH);
  delay(MESSAGE_START_PULSE);
  digitalWrite(LASER_PIN, LOW);
  delay(GAP_BETWEEN_LETTERS);

  for (int i = 0; i < message.length(); i++) {
    sendByte(message[i]);
  }

  digitalWrite(LASER_PIN, LOW);
  delay(MESSAGE_END_PAUSE);
  Serial.println("--- Message complete, repeating soon ---");
  delay(2000);
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