#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define pins
#define SS_PIN 10
#define RST_PIN 9
#define RED_LED_PIN 3
#define GREEN_LED_PIN 4
#define BUTTON_PIN 6

// Create instances
MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Product structure and list (UID, name, and price)
struct Product {
  String uid;
  String name;
  float price;
};

Product products[] = {
  {"04 A7 53 6B 3D 24 80", "Product 1", 12.50},
  {"04 B9 C7 6A 3C 21 80", "Product 2", 7.25},
  {"04 E1 83 6B 2E 22 81", "Product 3", 15.00}
};

const int numProducts = sizeof(products) / sizeof(products[0]);
float totalBill = 0.0;

// List to keep track of scanned products
struct ScannedProduct {
  String uid;
  float price;
};

ScannedProduct scannedProducts[50];
int scannedCount = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Please put your card..");
  SPI.begin();
  rfid.PCD_Init();
  lcd.begin(16, 2);
  lcd.backlight();   // Turn on the backlight
  lcd.print("Smart Cart");
  lcd.setCursor(0, 1);
  lcd.print("Ready to scan");

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Use internal pull-up resistor
}

void loop() {
  // RFID Tag Detection
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String tagUID = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      tagUID += String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
      tagUID += String(rfid.uid.uidByte[i], HEX);
    }
    rfid.PICC_HaltA();

    Serial.print("Detected UID: ");
    Serial.println(tagUID);

    bool found = false;
    for (int i = 0; i < numProducts; i++) {
      if (tagUID.substring(1) == products[i].uid) {
        found = true;
        bool alreadyScanned = false;
        for (int j = 0; j < scannedCount; j++) {
          if (scannedProducts[j].uid == products[i].uid) {
            // Remove the product
            totalBill -= scannedProducts[j].price;
            // Shift remaining products in the list
            for (int k = j; k < scannedCount - 1; k++) {
              scannedProducts[k] = scannedProducts[k + 1];
            }
            scannedCount--;
            lcd.clear();
            lcd.print("Removed: ");
            lcd.print(products[i].name);
            delay(2000);
            alreadyScanned = true;
            break;
          }
        }
        if (!alreadyScanned) {
          // Add the product
          totalBill += products[i].price;
          scannedProducts[scannedCount].uid = products[i].uid;
          scannedProducts[scannedCount].price = products[i].price;
          scannedCount++;
          lcd.clear();
          lcd.print("Added: ");
          lcd.print(products[i].name);
          delay(2000);
        }
        lcd.clear();
        lcd.print("Total Bill: $");
        lcd.print(totalBill);
        break;
      }
    }
    if (!found) {
      lcd.clear();
      lcd.print("Unknown Product");
      delay(2000);
      lcd.clear();
      lcd.print("Smart Cart");
      lcd.setCursor(0, 1);
      lcd.print("Ready to scan");
    }
  }

  // Push Button Press Detection
  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW) {
    lcd.clear();
    lcd.print("Total Bill: $");
    lcd.print(totalBill);
    digitalWrite(RED_LED_PIN, HIGH);  // Turn on red LED
    delay(2000);  // Wait for 2 seconds
    digitalWrite(RED_LED_PIN, LOW);  // Turn off red LED

    lcd.clear();
    lcd.print("Smart Cart");
    lcd.setCursor(0, 1);
    lcd.print("Ready to scan");
  }
}
