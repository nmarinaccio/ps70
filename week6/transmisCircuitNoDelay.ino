// ESP32 LED -> Photoresistor ASCII string TX/RX (non-blocking millis version)

const int ledPin = 23;
const int photoresistorPin = 27;

const int THRESH_LOW_IS_ONE   = 1850;
const int THRESH_HIGH_IS_ZERO = 1950;
const unsigned long settleMs  = 20;
const int analogSamples       = 8;

const char* MESSAGE = "Now I have to go back to work on my State of the Union speech, and I worked on it till pretty late last night, but want to say one thing to the American people...";

// ---------- Helper functions ----------
int readAnalogAvg(int pin, int samples) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(pin);
    delayMicroseconds(500);
  }
  return (int)(sum / samples);
}

int decideBit(int analogVal, int lastBit) {
  if (analogVal < THRESH_LOW_IS_ONE)   return 1;
  if (analogVal > THRESH_HIGH_IS_ZERO) return 0;
  return lastBit;
}

// ---------- Communication class ----------
class LedPhotoSerial {
public:
  LedPhotoSerial(int led, int sensor)
    : ledPin(led), sensorPin(sensor), state(IDLE), msgIndex(0),
      bitIndex(7), lastBit(0), currentByte(0), lastTime(0) {}

  void begin() {
    pinMode(ledPin, OUTPUT);
    pinMode(sensorPin, INPUT);
  }

  void loop() {
    unsigned long now = millis();

    switch (state) {
      case IDLE:
        if (MESSAGE[msgIndex] != '\0') {
          currentByte = MESSAGE[msgIndex];
          bitIndex = 7;
          state = SENDING_BIT;
          lastTime = now;
        } else {
          if (now - lastTime > 2000) {  // wait before restarting
            msgIndex = 0;
            Serial.println();
            lastTime = now;
          }
        }
        break;

      case SENDING_BIT:
        if (now - lastTime >= settleMs) {
          int bitToSend = bitRead(currentByte, bitIndex);
          digitalWrite(ledPin, bitToSend ? HIGH : LOW);
          lastTime = now;
          state = READING_BIT;
        }
        break;

      case READING_BIT:
        if (now - lastTime >= settleMs) {
          int analogVal = readAnalogAvg(sensorPin, analogSamples);
          int rxBit = decideBit(analogVal, lastBit);
          lastBit = rxBit;

          if (rxBit) receivedByte |= (1 << bitIndex);
          else       receivedByte &= ~(1 << bitIndex);

          if (bitIndex == 0) {
            Serial.write(receivedByte);
            digitalWrite(ledPin, LOW);
            state = INTERBYTE_DELAY;
            lastTime = now;
          } else {
            bitIndex--;
            state = SENDING_BIT;
            lastTime = now;
          }
        }
        break;

      case INTERBYTE_DELAY:
        if (now - lastTime >= 75) { // half delay between chars
          msgIndex++;
          state = IDLE;
          lastTime = now;
        }
        break;
    }
  }

private:
  enum State { IDLE, SENDING_BIT, READING_BIT, INTERBYTE_DELAY };
  int ledPin, sensorPin;
  State state;
  size_t msgIndex;
  int bitIndex;
  int lastBit;
  uint8_t currentByte;
  uint8_t receivedByte;
  unsigned long lastTime;
};

// ---------- Global object ----------
LedPhotoSerial comm(ledPin, photoresistorPin);

void setup() {
  Serial.begin(115200);
  comm.begin();
}

void loop() {
  comm.loop(); // non-blocking main loop
}
