#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <LiquidCrystal_I2C.h>

// Fixed thresholds
const int lightThreshold = 300; // Adjust for ambient light conditions
const int tempThreshold = 30;   // Temperature threshold in °C

// Define components
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_600MS, TCS34725_GAIN_1X);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust 0x27 to your LCD's I2C address
const int whiteLEDPin = 7;

// DS1621 configuration
#define DS1621_ADDRESS 0x48

// Function to read DS1621 temperature
int readTemperature() {
  Wire.beginTransmission(DS1621_ADDRESS);
  Wire.write(0xAA); // Command to read temperature
  if (Wire.endTransmission() != 0) {
    return -1; // Error in communication
  }

  Wire.requestFrom(DS1621_ADDRESS, 1);
  if (Wire.available()) {
    return Wire.read(); // Read temperature
  }
  return -1; // No data received
}

void setup() {
  Wire.begin();
  Serial.begin(9600);
  lcd.begin(16, 2); // 16 columns, 2 rows
  lcd.backlight();

  if (!tcs.begin()) {
    lcd.print("TCS3472 Error!");
    while (1);
  }

  // Initialize DS1621
  Wire.beginTransmission(DS1621_ADDRESS);
  Wire.write(0xAC); // Access Configuration Register
  Wire.write(0x00); // Continuous conversion mode
  if (Wire.endTransmission() != 0) {
    lcd.print("DS1621 Init Fail!");
    while (1);
  }

  pinMode(whiteLEDPin, OUTPUT);
}

void loop() {
  int temperature = readTemperature();
  if (temperature == -1) {
    lcd.setCursor(0, 0);
    lcd.print("Temp Read Error");
    delay(1000);
    return;
  }

  // Read TCS3472 ambient light
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
  uint16_t brightness = c;

  // Adjust White LED based on thresholds
  if (brightness < lightThreshold || temperature > tempThreshold) {
    digitalWrite(whiteLEDPin, HIGH); // LED ON for low light or high temperature
  } else {
    digitalWrite(whiteLEDPin, LOW); // LED OFF
  }

  // Display data on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Light: ");
  lcd.print(brightness);

  // Send data to Arduino 2
  Serial.print(temperature);
  Serial.print(",");
  Serial.println(brightness);

  delay(1000);
}
