int ThermistorPin1 = A10;   // Left sensor
int ThermistorPin2 = A11;   // Right sensor

const float ADC_MAX = 1023.0;

// Breath detection settings
float smoothLeft = 0;
float smoothRight = 0;

float baselineLeft = 0;
float baselineRight = 0;

float smoothFactor = 0.15;      // higher = more responsive, lower = smoother
float baselineFactor = 0.005;   // slow baseline tracking

int threshold = 8;              // adjust based on your signal size

bool inBreathLeft = false;
bool inBreathRight = false;

unsigned long lastBreathTimeLeft = 0;
unsigned long lastBreathTimeRight = 0;

unsigned long minBreathInterval = 1200; // ignore breaths faster than 1.2 sec apart

int breathCountLeft = 0;
int breathCountRight = 0;

unsigned long bpmWindowStart = 0;
const unsigned long bpmWindow = 30000; // 30 sec window

float bpmLeft = 0;
float bpmRight = 0;

void setup() {
  Serial.begin(9600);

  int rawLeft = analogRead(ThermistorPin1);
  int rawRight = analogRead(ThermistorPin2);

  smoothLeft = rawLeft;
  smoothRight = rawRight;

  baselineLeft = rawLeft;
  baselineRight = rawRight;

  bpmWindowStart = millis();
}

void loop() {
  int rawLeft = analogRead(ThermistorPin1);
  int rawRight = analogRead(ThermistorPin2);

  // Smooth the raw signals
  smoothLeft = smoothLeft + smoothFactor * (rawLeft - smoothLeft);
  smoothRight = smoothRight + smoothFactor * (rawRight - smoothRight);

  // Slowly update baselines
  baselineLeft = baselineLeft + baselineFactor * (smoothLeft - baselineLeft);
  baselineRight = baselineRight + baselineFactor * (smoothRight - baselineRight);

  // Difference from baseline
  float signalLeft = smoothLeft - baselineLeft;
  float signalRight = smoothRight - baselineRight;

  unsigned long now = millis();

  // LEFT breath detection
  if (!inBreathLeft && signalLeft > threshold && (now - lastBreathTimeLeft > minBreathInterval)) {
    breathCountLeft++;
    lastBreathTimeLeft = now;
    inBreathLeft = true;
  }

  if (inBreathLeft && signalLeft < 0) {
    inBreathLeft = false;
  }

  // RIGHT breath detection
  if (!inBreathRight && signalRight > threshold && (now - lastBreathTimeRight > minBreathInterval)) {
    breathCountRight++;
    lastBreathTimeRight = now;
    inBreathRight = true;
  }

  if (inBreathRight && signalRight < 0) {
    inBreathRight = false;
  }

  // Calculate BPM every 30 seconds
  if (now - bpmWindowStart >= bpmWindow) {
    bpmLeft = breathCountLeft * (60000.0 / bpmWindow);
    bpmRight = breathCountRight * (60000.0 / bpmWindow);

    breathCountLeft = 0;
    breathCountRight = 0;
    bpmWindowStart = now;
  }

  // Serial Plotter output
  Serial.print("RawLeft:");
  Serial.print(rawLeft);

  Serial.print(" RawRight:");
  Serial.print(rawRight);

  Serial.print(" SignalLeft:");
  Serial.print(signalLeft);

  Serial.print(" SignalRight:");
  Serial.print(signalRight);

  Serial.print(" Threshold:");
  Serial.print(threshold);

  Serial.print(" BPMLeft:");
  Serial.print(bpmLeft);

  Serial.print(" BPMRight:");
  Serial.println(bpmRight);

  delay(50);
}