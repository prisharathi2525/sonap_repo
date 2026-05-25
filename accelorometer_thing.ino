const int xpin = A2;
const int ypin = A1;
const int zpin = A0;

void setup() {
  Serial.begin(9600);
  pinMode(xpin, INPUT); 
  pinMode(ypin, INPUT); 
  pinMode(zpin, INPUT); 
}

void loop() {
  // print the sensor values:  
  Serial.print("X: ");
  double xVal = ((analogRead(xpin))*5.00/1024.00 - 1.65)/.33;
  Serial.println(xVal);

 Serial.print("Y: ");
 double yVal = ((analogRead(ypin))*5.00/1024.00 - 1.65)/.33;
  Serial.println(yVal);
  Serial.print("Z: ");
 double zVal = ((analogRead(zpin))*5.00/1024.00 - 1.65)/.33;
  Serial.println(zVal);

  Serial.print("Roll: ");
float roll = (((atan2(yVal, zVal) * 180 / 3.14) + 180));
Serial.println(roll);

Serial.print("Pitch: ");
float pitch = (((atan2(xVal, zVal) * 180) / 3.14) + 180);
Serial.println(pitch);

Serial.print("Yaw: ");
float yaw = (((atan2(-yVal, xVal) * 180) / 3.14) + 180);
Serial.println(yaw);

  Serial.println(); 

// focus on below
 // if (roll > 155 or roll < 200){
    if (yaw > 130) {
    Serial.println("Final: right");
    } else if (yaw > 115 and yaw < 130) {
      Serial.println("Final: upright");
    } else if (yaw < 115){
      Serial.println("Final: left");
    }
 // } else if (roll < 155 or roll > 200) {
    if (pitch > 220 and pitch < 230){
    Serial.println("Final: upright");
    } else if (pitch > 230) {
    Serial.println("Final: prone");
    } else if (pitch < 220){
    Serial.println("Final: supine");
    }
 // }
    
 // delay before next reading:  
  delay(500);
}
