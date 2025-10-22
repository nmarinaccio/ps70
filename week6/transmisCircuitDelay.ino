// ESP32 LED -> Photoresistor ASCII string TX/RX (2× speed version)
// Sends an ASCII string via LED flashes, reads it, and prints characters as received.

const int ledPin = 23;
const int photoresistorPin = 27;

// ------- thresholds & timing -------
const int THRESH_LOW_IS_ONE   = 1850;  // analog < this => '1'
const int THRESH_HIGH_IS_ZERO = 1950;  // analog > this => '0'
const unsigned long settleMs  = 20;    // ↓ halved from 40ms → 2× speed
const int analogSamples       = 8;     // average samples per bit

// ------- hidden message -------
const char* MESSAGE = "Now I have to go back to work on my State of the Union speech, and I worked on it till pretty late last night, but  want to say one thing to the American people, I want you to listen to me, I’m going to say this again, I did not have sexual relations with that woman, Miss Lewinsky, I never told anybody to lie, not a single time, never. These allegations are false, and I need to go back to work for the American people. Thank you.";

// ------- helpers -------
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

uint8_t txrxByte(uint8_t b, int &lastBitState) {
  uint8_t received = 0;
  for (int bit = 7; bit >= 0; bit--) {
    int bitToSend = bitRead(b, bit);
    digitalWrite(ledPin, bitToSend ? HIGH : LOW);
    delay(settleMs);
    int analogVal = readAnalogAvg(photoresistorPin, analogSamples);
    int rxBit = decideBit(analogVal, lastBitState);
    lastBitState = rxBit;
    if (rxBit) received |= (1 << bit);
    else       received &= ~(1 << bit);
  }
  digitalWrite(ledPin, LOW);
  delay(settleMs);  // shorter pause between bytes
  return received;
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(photoresistorPin, INPUT);
}

void loop() {
  int lastBit = 0;
  for (size_t i = 0; MESSAGE[i] != '\0'; i++) {
    uint8_t sent = static_cast<uint8_t>(MESSAGE[i]);
    uint8_t got = txrxByte(sent, lastBit);
    Serial.write(got);  // print as received
    delay(75);          // ↓ half delay between characters for smooth output
  }
  Serial.println();
  delay(2000);
}
