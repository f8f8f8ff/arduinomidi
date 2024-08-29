#include "midi_mixer.h"
#include "sensor_manager.h"

midi_mixer mixer;

void setup() {
  sensor_manager::add_sensor(&hcsr04(8, 9, 220));
  mixer.set_channel_range(0, 10, 220);
  Serial.begin(9600);
}

void loop() {
  sensor_manager::read();
  if (mixer.check_timer()) {
    sensor_manager::process();
    for (int i = 0; i < sensor_manager::num_sensors(); i++) {
      int value = sensor_manager::value(i);
      mixer.input(value, i);
      if (false) {
        Serial.print("loop() ");
        Serial.print(i);
        Serial.print(" ");
        Serial.print(value);
        Serial.print("\n");
      }
    }
    mixer.interpolate();
    mixer.mix();
    mixer.flush();
  }
}
