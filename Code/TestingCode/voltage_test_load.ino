#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int counter = 0;

void setup()
{
    Wire.begin(8, 9);
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Voltage Test Load");
    lcd.setCursor(0, 1);
    lcd.print("Running...");
}

void loop()
{
    lcd.setCursor(10, 1);
    lcd.print(counter);
    counter++;
    delay(1000);
}
