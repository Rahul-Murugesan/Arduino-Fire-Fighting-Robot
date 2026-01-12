#include <Servo.h>

// ================= MOTOR DRIVER PINS =================
#define enA 5     // Enable A
#define in1 9      // Motor A1
#define in2 8      // Motor A2

#define in3 7      // Motor B1
#define in4 6      // Motor B2
#define enB 11      // Enable B

// ================= SENSOR PINS =================
#define ir_R A0
#define ir_F A1
#define ir_L A2

// ================= SERVO =================
#define SERVO_PIN 3

Servo sprayServo;

// ================= VARIABLES =================
int Speed = 180;           // Motor speed
int FIRE_THRESHOLD = 200;  // Fire when value < 200

// Servo sweep variables
int servoPos = 0;
int sweepCount = 0;
bool sweeping = false;
unsigned long lastServoMove = 0;
const int servoInterval = 5;   // Sweep speed

#define BUZZER_PIN 4  // Connect your buzzer here

void setup() {
  Serial.begin(9600);
  
  // Motor Pins
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enB, OUTPUT);

  // Sensor Pins
  pinMode(ir_R, INPUT);
  pinMode(ir_F, INPUT);
  pinMode(ir_L, INPUT);

  analogWrite(enA, Speed);
  analogWrite(enB, Speed);

  // Servo setup
  sprayServo.attach(SERVO_PIN);
  sprayServo.write(0);

  // Buzzer setup
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH);
}


void loop() {
  int sR = analogRead(ir_R);
  int sF = analogRead(ir_F);
  int sL = analogRead(ir_L);

  // Serial.print("R:");
  // Serial.print(sR);
  // Serial.print("  F:");
  // Serial.print(sF);
  // Serial.print("  L:");
  // Serial.println(sL);

  bool fireR = (sR < FIRE_THRESHOLD);
  bool fireF = (sF < FIRE_THRESHOLD);
  bool fireL = (sL < FIRE_THRESHOLD);

  // ================= FIRE DETECTION =================
  if ((fireR || fireF || fireL) && !sweeping) {

    Serial.println("🔥 Fire Detected");

    // Move toward fire
    if (fireL && !fireF) {
      Serial.println("↩ Turning LEFT");
      turnLeft();
      delay(300);
      stopMotor();
      sweeping = true;  // start servo sweep
    }
    else if (fireR && !fireF) {
      Serial.println("↪ Turning RIGHT");
      turnRight();
      delay(300);
      stopMotor();
      sweeping = true;  // start servo sweep
    }
    else if (fireF) {
      Serial.println("⬆ Fire in FRONT - STOP & SWEEP");
      forward();
      delay(300);
      stopMotor();
      sweeping = true;  // start servo sweep
    }
  }
  else if (!sweeping) {
    // No fire → keep moving forward
    
  }

  servoSpray();  // Handle servo motion
  delay(100);
}

// ================= SERVO SWEEP FUNCTION =================
void servoSpray() {
  if (!sweeping) return;

  static int direction = 1;
  const int step = 5;          // Move 5 degrees per update
  const unsigned long interval = 5;  // 5 ms between moves

  unsigned long currentMillis = millis();
  if (currentMillis - lastServoMove >= interval) {
    lastServoMove = currentMillis;

    // Turn buzzer ON when sweep starts
    if (sweepCount == 0 && servoPos == 0) {
      digitalWrite(BUZZER_PIN, HIGH);
      Serial.println("🔊 Buzzer ON - Sweep started");
    }

    servoPos += step * direction;
    
    if (servoPos >= 90) {
      servoPos = 90;
      direction = -1;
      sweepCount++;
    } 
    else if (servoPos <= 0) {
      servoPos = 0;
      direction = 1;
      sweepCount++;
    }

    sprayServo.write(servoPos);

    // Stop after 2 full sweeps
    if (sweepCount >= 2) {
      sweeping = false;
      sweepCount = 0;
      servoPos = 0;
      sprayServo.write(0);

      // Turn buzzer ON momentarily to signal sweep complete
      Serial.println("✅ Sweep Completed");
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);             // beep for 0.5 sec
      digitalWrite(BUZZER_PIN, LOW);
      Serial.println("🔊 Buzzer OFF - Sweep finished");
    }
  }
}


// ================= MOTOR FUNCTIONS =================
void forward() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
   Serial.println("↪ Turning forward");
}

void backward() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
   Serial.println("↪ Turning bacck");
}

void turnRight() {
  digitalWrite(in1, LOW);  
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);  
  digitalWrite(in4, HIGH);
   Serial.println("↪ Turning RIGHT");
}

void turnLeft() {
  digitalWrite(in1, HIGH); 
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH); 
  digitalWrite(in4, LOW);
   Serial.println("↪ Turning left");
}

void stopMotor() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
   Serial.println("↪ Turning sTop");
}
