// --- pins (avoid GPIO1/TX0 for LED) ---
#define GREEN_LED 21    // moved off pin 1 (TX0) to prevent it being stuck HIGH
#define YELLOW_LED 22
#define RED_LED    23

#define POT_pin    15

// motor pins
#define B1A 12
#define B1B 14

// PWM props (keep your ledcAttach/ledcWrite usage)
const int freq = 5000;
const int resolution = 8;

// globals
int potVal = 0;
int speed_mapped = 0;

void setup() {
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);

  pinMode(POT_pin, INPUT);

  pinMode(B1A, OUTPUT);
  pinMode(B1B, OUTPUT);

  ledcAttach(B1A, freq, resolution);
  ledcWrite(B1A, 0);
  digitalWrite(B1B, LOW);

  Serial.begin(9600);
}

void loop() {
  potVal = analogRead(POT_pin);                // ESP32 default: 0–4095
  speed_mapped = map(potVal, 0, 4095, 0, 255); // duty 0–255
  speed_mapped = constrain(speed_mapped, 0, 255);

  // drive motor
  ledcWrite(B1A, speed_mapped);
  digitalWrite(B1B, LOW); // fixed direction

  // LED logic by thirds
  if (speed_mapped == 0) {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
  } else if (speed_mapped <= 85) {           // lower third (1..85)
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
  } else if (speed_mapped <= 170) {          // middle third (86..170)
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, LOW);
  } else {                                   // top third (171..255)
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, HIGH);
  }

  // debug (optional)
  Serial.println(speed_mapped);
}
