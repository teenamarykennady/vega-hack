#include<>
void setup(){}
void loop()
{digitalWrite(ledpin,HIGH)}

#include <Servo.h>

#define GREEN_LED 7
#define YELLOW_LED 6
#define RED_LED 5
#define BUZZER 8
#define SERVO_PIN 3
#define TOUCH_PIN 2
#define LDR A0

Servo lidServo;

bool systemON = false;
bool monitoring = false;

float riskScore = 0;
unsigned long lastUpdate = 0;
unsigned long lidOpenStart = 0;
bool lastTouchState = LOW;

void setup() {
  Serial.begin(115200);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(TOUCH_PIN, INPUT);

  lidServo.attach(SERVO_PIN);
  lidServo.write(0);

  Serial.println("FoodShield Ready");
}

void loop() {

  // ===== TOUCH SENSOR BUTTON =====
  bool touchState = digitalRead(TOUCH_PIN);

  if(touchState == HIGH && lastTouchState == LOW){
    delay(50);
    systemON = !systemON;
    monitoring = false;
    riskScore = 0;

    if(systemON){
      Serial.println("SYSTEM STARTED");
      lidOpenStart = millis();
      digitalWrite(BUZZER, HIGH); delay(200); digitalWrite(BUZZER, LOW);

      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(YELLOW_LED, LOW);
      digitalWrite(RED_LED, LOW);
    }
    else{
      Serial.println("SYSTEM STOPPED");
      lidServo.write(0);
      digitalWrite(BUZZER, LOW);

      digitalWrite(GREEN_LED, LOW);
      digitalWrite(YELLOW_LED, LOW);
      digitalWrite(RED_LED, LOW);
    }
  }
  lastTouchState = touchState;

  if(!systemON) return;

  // ===== RFID SIMULATION (press r) =====
  if(!monitoring && Serial.available()){
    char c = Serial.read();
    if(c == 'r' || c == 'R'){
      monitoring = true;
      riskScore = 0;
      Serial.println("Food Batch Registered");
    }
  }

  if(!monitoring) return;

  // ===== FAST DEMO TIMER (updates every 5 sec) =====
  if(millis() - lastUpdate > 5000){
    lastUpdate = millis();

    int lightValue = analogRead(LDR);

    Serial.print("Light Sensor: ");
    Serial.println(lightValue);

    Serial.print("Risk Score: ");
    Serial.println(riskScore);

    riskScore += 15;
  }

  // ===== LED + BUZZER STATES =====
  if(riskScore < 30){
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);
  }
  else if(riskScore < 60){
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);   // buzzer ON
  }
  else{
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER, HIGH);   // buzzer ON (danger)
  }

  // ===== SERVO AUTO CLOSE AFTER 45 sec =====
  if(millis() - lidOpenStart > 45000){
    lidServo.write(90);
  }
}
