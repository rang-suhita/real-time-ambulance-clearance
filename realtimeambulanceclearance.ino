#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);

// LED Pins
#define RED 2
#define YELLOW 3
#define GREEN 4

// Buttons
#define EMERGENCY_BUTTON 5
#define RESET_BUTTON 6

// Buzzer
#define BUZZER 7

// Ambulance UID (Card)
String ambulanceUID = "83 74 EF 13";

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);

  pinMode(BUZZER, OUTPUT);

  pinMode(EMERGENCY_BUTTON, INPUT_PULLUP);
  pinMode(RESET_BUTTON, INPUT_PULLUP);

  Serial.println("System Ready...");
}

void loop() {

  // 🔘 Manual Emergency Button
  if (digitalRead(EMERGENCY_BUTTON) == LOW) {
    grantPriority();
  }

  // 🔘 Reset Button
  if (digitalRead(RESET_BUTTON) == LOW) {
    normalTraffic();
  }

  // 🚑 RFID Detection
  if (!mfrc522.PICC_IsNewCardPresent()) {
    normalTraffic();
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  String readUID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    readUID += String(mfrc522.uid.uidByte[i], HEX);
    readUID += " ";
  }

  readUID.toUpperCase();
  readUID.trim();

  Serial.print("Scanned UID: ");
  Serial.println(readUID);

  if (readUID == ambulanceUID) {
    Serial.println("Ambulance Detected!");
    grantPriority();
  } else {
    Serial.println("Normal Vehicle");
    normalTraffic();
  }

  mfrc522.PICC_HaltA();
}

// 🚦 Normal Traffic Sequence
void normalTraffic() {
  digitalWrite(GREEN, HIGH);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, LOW);
  digitalWrite(BUZZER, LOW);
  delay(3000);

  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, HIGH);
  delay(1000);

  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, HIGH);
  delay(3000);
}

// 🚑 Emergency Override
void grantPriority() {

  // Turn OFF all lights first
  digitalWrite(RED, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(GREEN, LOW);

  // Emergency Green ON
  digitalWrite(GREEN, HIGH);

  // 🔔 Buzzer ON only for alert (3 seconds)
  digitalWrite(BUZZER, HIGH);
  delay(3000);
  digitalWrite(BUZZER, LOW);

  // 🚑 Keep green ON longer for ambulance passing
  delay(7000);

  // After ambulance passes, turn green OFF
  digitalWrite(GREEN, LOW);
}