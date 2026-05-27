#include <Wire.h>
#include "MAX30105.h"

MAX30105 particleSensor;

// ── Accelerometer pins ──────────────────────────────────────────────
const int xpin = A2;
const int ypin = A1;
const int zpin = A0;

// ── Thermistor pins ────────────────────────────────────────────────
const int ThermistorPin1 = A10;
const int ThermistorPin2 = A11;

// ── Setup ──────────────────────────────────────────────────────────
void setup() {

  Serial.begin(115200);
  Wire.begin();

  // Accelerometer pins
  pinMode(xpin, INPUT);
  pinMode(ypin, INPUT);
  pinMode(zpin, INPUT);

  // Verify MAX30102 connection
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102_NOT_FOUND");
    while (1);
  }

  // Configure MAX30102
  particleSensor.setup(
    0x1F,  // LED brightness
    4,     // sample average
    2,     // Red + IR
    100,   // sample rate
    411,   // pulse width
    4096   // ADC range
  );

  Serial.println("READY");
}

// ── Loop ───────────────────────────────────────────────────────────
void loop() {

  // Raw thermistor values
  int rawLeft  = analogRead(ThermistorPin1);
  int rawRight = analogRead(ThermistorPin2);

  // Raw accelerometer values
  int xRaw = analogRead(xpin);
  int yRaw = analogRead(ypin);
  int zRaw = analogRead(zpin);

  // Raw MAX30102 values
  long irValue  = particleSensor.getIR();
  long redValue = particleSensor.getRed();

  // CSV output only
  Serial.print(millis());   // t
  Serial.print(",");

  Serial.print(rawLeft);    // Tleft
  Serial.print(",");

  Serial.print(rawRight);   // Tright
  Serial.print(",");

  Serial.print(irValue);    // IR
  Serial.print(",");

  Serial.print(redValue);   // Red
  Serial.print(",");

  Serial.print(xRaw);       // AccX
  Serial.print(",");

  Serial.print(yRaw);       // AccY
  Serial.print(",");

  Serial.println(zRaw);     // AccZ

  delay(50);
}
