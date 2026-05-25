#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

MAX30105 particleSensor;

#define BUFFER_SIZE 100

uint32_t irBuffer[BUFFER_SIZE];
uint32_t redBuffer[BUFFER_SIZE];

int32_t spo2;
int8_t validSPO2;

int32_t heartRate;
int8_t validHeartRate;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 not found");
    while (1);
  }

  Serial.println("Starting SpO2 Plotter...");

  particleSensor.setup(
    0x3F,  // LED brightness
    4,     // sample average
    2,     // Red + IR
    100,   // sample rate
    411,   // pulse width
    4096   // ADC range
  );
}

void loop() {

  // Gather samples
  for (byte i = 0; i < BUFFER_SIZE; i++) {

    while (particleSensor.available() == false) {
      particleSensor.check();
    }

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();

    particleSensor.nextSample();
  }

  // Run algorithm
  maxim_heart_rate_and_oxygen_saturation(
    irBuffer,
    BUFFER_SIZE,
    redBuffer,
    &spo2,
    &validSPO2,
    &heartRate,
    &validHeartRate
  );

  // Output for Serial Plotter
  if (validSPO2 && spo2 > 0 && spo2 <= 100) {

    Serial.print("SpO2:");
    Serial.print(spo2);

    Serial.print(",");

    // Optional raw IR signal for waveform
    Serial.print("IR:");
    Serial.println(irBuffer[BUFFER_SIZE - 1]);

  } else {

    Serial.print("SpO2:0");
    Serial.print(",");

    Serial.print("IR:");
    Serial.println(irBuffer[BUFFER_SIZE - 1]);
  }

  delay(100);
}