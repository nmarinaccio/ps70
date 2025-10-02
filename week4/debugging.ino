#define POT_PIN 15

#define A1A 12
#define A1B 14

const int freq = 5000;
const int resolution = 8;

void setup() {
  pinMode(POT_PIN, INPUT);
  pinMode(A1A, OUTPUT);
  pinMode(A1B, OUTPUT);

  ledcAttach(A1A, freq, resolution);
  ledcWrite(A1A, 0);
  digitalWrite(A1B, LOW);
  Serial.begin(115200);
}

void loop() {
  int potVal = analogRead(POT_PIN);            // 0–4095 on ESP32
  int duty   = map(potVal, 0, 4095, 0, 255);   // 0–255 duty
  ledcWrite(A1A, duty);
  digitalWrite(A1B, LOW);
  Serial.println(duty);

}
