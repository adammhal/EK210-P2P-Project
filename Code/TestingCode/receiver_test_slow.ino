#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x20, 16, 2);

const int RX_PIN = 6;
const int LED_PIN = 10;

const int MESSAGE_START_TIME = 1950;
const int MESSAGE_END_TIME = 1950;
const int BYTE_START_PULSE = 500;
const int BIT_DURATION = 250;
const int GAP_AFTER_BYTE_START = 250;
const int GAP_BETWEEN_LETTERS = 500;

void setup()
{
    Serial.begin(9600);
    pinMode(RX_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    Wire.begin(8, 9);
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Speed Test B (Fast)");
}

int readRxPin()
{
    int state = digitalRead(RX_PIN);
    digitalWrite(LED_PIN, (state == LOW) ? HIGH : LOW);
    return state;
}

void loop()
{
    waitForStartSignal();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Receiving...");

    unsigned long startTime = millis();

    while (true)
    {
        byte b = receiveByte();
        if (b == 0)
        {
            break;
        }
    }

    unsigned long totalTime = millis() - startTime;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Time (ms):");
    lcd.setCursor(0, 1);
    lcd.print(totalTime);
    Serial.print("Total Time (Fast): ");
    Serial.println(totalTime);

    delay(3000);
}

void waitForStartSignal()
{
    lcd.setCursor(0, 1);
    lcd.print("Waiting...");
    while (true)
    {
        while (readRxPin() == HIGH)
        {
        }
        unsigned long startTime = millis();
        while (readRxPin() == LOW)
        {
        }
        unsigned long duration = millis() - startTime;
        if (duration > MESSAGE_START_TIME)
        {
            return;
        }
    }
}

byte receiveByte()
{
    while (true)
    {
        unsigned long waitStartTime = millis();
        while (readRxPin() == HIGH)
        {
            if (millis() - waitStartTime > MESSAGE_END_TIME)
            {
                return 0;
            }
        }
        unsigned long blinkStartTime = millis();
        while (readRxPin() == LOW)
        {
        }
        unsigned long blinkDuration = millis() - blinkStartTime;

        if (blinkDuration > (BYTE_START_PULSE - 150) && blinkDuration < (BYTE_START_PULSE + 150))
        {
            delay(GAP_AFTER_BYTE_START);
            byte receivedData = 0;

            for (int i = 7; i >= 0; i--)
            {
                delay(BIT_DURATION / 2);
                int bitState = readRxPin();
                if (bitState == LOW)
                {
                    receivedData |= (1 << i);
                }
                delay(BIT_DURATION / 2);
            }

            lcd.setCursor(0, 1);
            lcd.print("Byte OK");

            delay(GAP_BETWEEN_LETTERS);
            return receivedData;
        }
    }
}
