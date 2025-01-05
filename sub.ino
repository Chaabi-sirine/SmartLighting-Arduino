#include <SPI.h>
#include <SD.h>

// Pin Definitions
const int chipSelect = 10; // SD card module CS pin
const int buttonPin = 2;   // Button pin
const int greenLEDPin = 8; // Green LED for activity

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP); // Enable internal pull-up resistor
  pinMode(greenLEDPin, OUTPUT);
  
  if (!SD.begin(chipSelect)) {
    Serial.println("SD Card Error");
    while (1);
  }
  Serial.println("SD Card Ready");
}

void loop() {
  // Blink green LED during data logging
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    
    // Save data to SD card
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.println(data);
      dataFile.close();
      digitalWrite(greenLEDPin, HIGH);
      delay(100);
      digitalWrite(greenLEDPin, LOW);
    } else {
      Serial.println("File Error");
    }
  }

  // Check button state
  if (digitalRead(buttonPin) == LOW) { // Button pressed
    File dataFile = SD.open("datalog.txt");
    if (dataFile) {
      Serial.println("Data Log:");
      while (dataFile.available()) {
        Serial.write(dataFile.read());
      }
      dataFile.close();
    } else {
      Serial.println("No Data Found");
    }
    delay(1000); // Debounce delay
  }
}
