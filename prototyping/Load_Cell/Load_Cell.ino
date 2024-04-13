#include <Arduino.h>
#include "HX711.h"
#include "soc/rtc.h"

// Load Cell //
#define LOADCELL_DOUT_PIN 31 // IO19
#define LOADCELL_SCK_PIN 30 // IO18
#define CALIBRATION_FACT 202.86
#define WAIT_TIME 5000

HX711 scale;
// // // // // //

void init_scale() {
  Serial.println("Initializing the scale");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);        
  scale.set_scale(CALIBRATION_FACT);
  scale.tare(); 
}

float read_scale() {
  float weight_meas = scale.get_units();

  scale.power_down();             
  delay(WAIT_TIME);
  scale.power_up();
  return weight_meas;
}

void setup() {
  Serial.begin(115200);
  init_scale();
}

void loop() {
  Serial.print("Reading:\t");
  Serial.print(read_scale(), 2);
}