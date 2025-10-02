// define LED pins
#define GREEN_LED 21
#define YELLOW_LED 22
#define RED_LED 23

// define POT pin
#define POT_pin 15

// define motor pins
#define B1A 11
#define B1B 10

// setting PWM properties
const int freq = 5000;
const int resolution = 8;

int potVal = 0;
int speed_mapped = 0;

class Motor 
{
  int pin1;
  int pin2;

public:
  Motor(int b1a, int b1b) : pin1(b1a), pin2(b1b) {
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
    ledcAttach(pin1, freq, resolution);
    ledcWrite(pin1, 0);
    digitalWrite(pin2, LOW);
  }

  void Update(int speed) {
    speed = constrain(speed, 0, 255);
    ledcWrite(pin1, speed);
    digitalWrite(pin2, LOW);
  }
};

class LED
{
  int LED_pin;
  int color; // 0 = green, 1 = yellow, 2 = red

public:
  LED(int pin, int clr) : LED_pin(pin), color(clr) {
    pinMode(LED_pin, OUTPUT);
    digitalWrite(LED_pin, HIGH);
  }

  void Update(int speed) {
    int mapped = map(speed, 0, 255, 0, 3);
    switch (mapped) {
      case 0:
        digitalWrite(LED_pin, LOW);
        break;
      case 1:
        digitalWrite(LED_pin, (color == 0 && speed > 0) ? HIGH : LOW);
        break;
      case 2:
        digitalWrite(LED_pin, ((color == 0 || color == 1) && speed > 0) ? HIGH : LOW);
        break;
      case 3:
        digitalWrite(LED_pin, (speed > 0) ? HIGH : LOW);
        break;
    }
  }
};

Motor spinner(B1A, B1B);
LED green(GREEN_LED, 0);
LED yellow(YELLOW_LED, 1);
LED red(RED_LED, 2);

void setup() {
  pinMode(POT_pin, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  potVal = analogRead(POT_pin);
  speed_mapped = map(potVal, 0, 4095, 0, 255);
  Serial.println(speed_mapped);
  spinner.Update(speed_mapped);
  green.Update(speed_mapped);
  yellow.Update(speed_mapped);
  red.Update(speed_mapped); 
}
