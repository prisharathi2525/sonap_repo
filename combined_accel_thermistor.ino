// ── Accelerometer pins ──────────────────────────────────────────────
const int xpin = A2;
const int ypin = A1;
const int zpin = A0;

// ── Thermistor pins & settings ───────────────────────────────────────
int ThermistorPin1 = A10;  // Left sensor
int ThermistorPin2 = A11;  // Right sensor

const float ADC_MAX = 1023.0;

float smoothLeft  = 0;
float smoothRight = 0;
float baselineLeft  = 0;
float baselineRight = 0;

float smoothFactor   = 0.15;   // higher = more responsive, lower = smoother
float baselineFactor = 0.005;  // slow baseline tracking

int threshold = 8;             // adjust based on your signal size

bool inBreathLeft  = false;
bool inBreathRight = false;

unsigned long lastBreathTimeLeft  = 0;
unsigned long lastBreathTimeRight = 0;
unsigned long minBreathInterval   = 1200; // ignore breaths faster than 1.2 sec apart

int breathCountLeft  = 0;
int breathCountRight = 0;

unsigned long bpmWindowStart = 0;
const unsigned long bpmWindow = 30000; // 30-sec BPM window

float bpmLeft  = 0;
float bpmRight = 0;

// ── Setup ────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  // Accelerometer pins
  pinMode(xpin, INPUT);
  pinMode(ypin, INPUT);
  pinMode(zpin, INPUT);

  // Thermistor initialisation
  int rawLeft  = analogRead(ThermistorPin1);
  int rawRight = analogRead(ThermistorPin2);

  smoothLeft   = rawLeft;
  smoothRight  = rawRight;
  baselineLeft  = rawLeft;
  baselineRight = rawRight;

  bpmWindowStart = millis();
}

// ── Loop ─────────────────────────────────────────────────────────────
void loop() {
  unsigned long now = millis();

  // ── Thermistor (runs every iteration, ~50 ms effective rate) ────────
  int rawLeft  = analogRead(ThermistorPin1);
  int rawRight = analogRead(ThermistorPin2);

  smoothLeft  = smoothLeft  + smoothFactor * (rawLeft  - smoothLeft);
  smoothRight = smoothRight + smoothFactor * (rawRight - smoothRight);

  baselineLeft  = baselineLeft  + baselineFactor * (smoothLeft  - baselineLeft);
  baselineRight = baselineRight + baselineFactor * (smoothRight - baselineRight);

  float signalLeft  = smoothLeft  - baselineLeft;
  float signalRight = smoothRight - baselineRight;

  // Left breath detection
  if (!inBreathLeft && signalLeft > threshold && (now - lastBreathTimeLeft > minBreathInterval)) {
    breathCountLeft++;
    lastBreathTimeLeft = now;
    inBreathLeft = true;
  }
  if (inBreathLeft && signalLeft < 0) {
    inBreathLeft = false;
  }

  // Right breath detection
  if (!inBreathRight && signalRight > threshold && (now - lastBreathTimeRight > minBreathInterval)) {
    breathCountRight++;
    lastBreathTimeRight = now;
    inBreathRight = true;
  }
  if (inBreathRight && signalRight < 0) {
    inBreathRight = false;
  }

  // BPM calculation every 30 seconds
  if (now - bpmWindowStart >= bpmWindow) {
    bpmLeft  = breathCountLeft  * (60000.0 / bpmWindow);
    bpmRight = breathCountRight * (60000.0 / bpmWindow);
    breathCountLeft  = 0;
    breathCountRight = 0;
    bpmWindowStart   = now;
  }

  // Thermistor serial output
  Serial.print("RawLeft:");    Serial.print(rawLeft);
  Serial.print(" RawRight:");  Serial.print(rawRight);
  Serial.print(" SignalLeft:"); Serial.print(signalLeft);
  Serial.print(" SignalRight:"); Serial.print(signalRight);
  Serial.print(" Threshold:"); Serial.print(threshold);
  Serial.print(" BPMLeft:");   Serial.print(bpmLeft);
  Serial.print(" BPMRight:");  Serial.println(bpmRight);

  // ── Accelerometer (runs every loop iteration, ~50 ms) ───────────────
  double xVal = ((analogRead(xpin)) * 5.00 / 1024.00 - 1.65) / .33;
  double yVal = ((analogRead(ypin)) * 5.00 / 1024.00 - 1.65) / .33;
  double zVal = ((analogRead(zpin)) * 5.00 / 1024.00 - 1.65) / .33;

  float roll  = (((atan2(yVal, zVal) * 180 / 3.14) + 180));
  float pitch = (((atan2(xVal, zVal) * 180) / 3.14) + 180);
  float yaw   = (((atan2(-yVal, xVal) * 180) / 3.14) + 180);

  Serial.print("X: ");     Serial.println(xVal);
  Serial.print("Y: ");     Serial.println(yVal);
  Serial.print("Z: ");     Serial.println(zVal);
  Serial.print("Roll: ");  Serial.println(roll);
  Serial.print("Pitch: "); Serial.println(pitch);
  Serial.print("Yaw: ");   Serial.println(yaw);

  // Orientation from yaw
  if (yaw > 130) {
    Serial.println("Final: right");
  } else if (yaw > 115 && yaw <= 130) {
    Serial.println("Final: upright");
  } else if (yaw < 115) {
    Serial.println("Final: left");
  }

  // Orientation from pitch
  if (pitch > 220 && pitch < 230) {
    Serial.println("Final: upright");
  } else if (pitch > 230) {
    Serial.println("Final: prone");
  } else if (pitch < 220) {
    Serial.println("Final: supine");
  }

  Serial.println();

  delay(50); // Main loop cadence — drives thermistor sampling rate
}
