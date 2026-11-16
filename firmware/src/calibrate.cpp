#include <Arduino.h>
#include "HX711.h"

const int DT_PIN = D2;
const int SCK_PIN = D4;
const float KNOWN_WEIGHT_G = 788;
const int CALIBRATION_SAMPLES = 30;

HX711 scale;

void waitForKey() {
  while (Serial.available()) Serial.read();   // clear old input
  Serial.println("Put weight on scale and press a key..");
  while (!Serial.available()) {}               // wait for a key
  Serial.println("key pressed..");
  while (Serial.available()) Serial.read();
}

void setup() {
  Serial.begin(115200);
  scale.begin(DT_PIN, SCK_PIN);

  scale.set_scale();
  scale.tare(CALIBRATION_SAMPLES);
  waitForKey();

  const float knownWeightRaw = scale.get_units(CALIBRATION_SAMPLES);
  Serial.print("Raw for known weight: ");
  Serial.println(knownWeightRaw);

  const float calibrationFactor = knownWeightRaw/KNOWN_WEIGHT_G;
  Serial.print("Calibration factor: ");
  Serial.println(calibrationFactor);

  scale.set_scale(calibrationFactor);
}

void loop() {
  Serial.print(scale.get_units(CALIBRATION_SAMPLES), 1);
  Serial.println(" g");
  delay(1000);
}
