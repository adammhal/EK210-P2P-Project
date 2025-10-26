// #include <Wire.h>

// void setup() {
//   // We MUST explicitly set the pins on the ESP32-S3
//   // before calling Wire.begin()
//   // Wire.begin(SDA_PIN, SCL_PIN)
//   Wire.begin(8, 9); 

//   Serial.begin(9600);
//   while (!Serial); // Wait for Serial Monitor to open
//   Serial.println("\nI2C Scanner");
// }

// void loop() {
//   byte error, address;
//   int nDevices;

//   Serial.println("Scanning...");
//   nDevices = 0;
//   for (address = 1; address < 127; address++) {
//     Wire.beginTransmission(address);
//     error = Wire.endTransmission();

//     if (error == 0) {
//       Serial.print("I2C device found at address 0x");
//       if (address < 16) {
//         Serial.print("0");
//       }
//       Serial.println(address, HEX);
//       nDevices++;
//     } else if (error == 4) {
//       Serial.print("Unknown error at address 0x");
//       if (address < 16) {
//         Serial.print("0");
//       }
//       Serial.println(address, HEX);
//     }
//   }
//   if (nDevices == 0) {
//     Serial.println("No I2C devices found\n");
//   } else {
//     Serial.println("Done\n");
//   }
//   delay(5000); // Scan every 5 seconds
// }