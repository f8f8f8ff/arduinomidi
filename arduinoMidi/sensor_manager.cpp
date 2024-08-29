#include "sensor_manager.h"

// returns new sensor count on success, -1 on error
int sensor_manager::add_sensor(hcsr04* sensor) {
    if (n_sensors >= MAX_SENSORS) return -1;
    if (sensor == nullptr) return -1;
    sensors[n_sensors++] = sensor;
    return n_sensors;
}

// returns value of sensor. process() needs to be called first.
int sensor_manager::value(size_t n) {
    if (n < 0 || n >= n_sensors) return -1;
    return values[n];
}

// read the sensors
void sensor_manager::read() { read_all(); }

size_t sensor_manager::num_sensors() { return n_sensors; }

static void sensor_manager::echo_check() {
    hcsr04* s = sensors[current_us_sensor];
    if (s->np.check_timer()) {
        s->current_reading = s->np.ping_result / US_ROUNDTRIP_CM;
        // Serial.print("got reading ");
        // Serial.print(s->current_reading);
        // Serial.print("\n");
    }
}

static void sensor_manager::read_sensor(size_t n) {
    static bool debug = false;
    if (n >= n_sensors) return;
    hcsr04* s = sensors[n];
    if (s == nullptr) return;
    unsigned int time = millis();
    // Serial.print("time ");
    // Serial.print(time);
    // Serial.print(" timer ");
    // Serial.println(s->timer);
    if (time >= s->timer) {
        s->timer = s->timer + (ping_interval * n_sensors);
        s->np.timer_stop();
        s->reading = s->current_reading;
        Serial.print("set reaeding ");
        Serial.println(s->reading);
        filter[n].add_reading(s->reading);
        current_us_sensor = n;
        s->current_reading = s->max_distance + 1;
        if (debug) {
            Serial.print("set current reading to max+1: ");
            Serial.print(s->current_reading);
            Serial.print("\n");
        }
        s->np.ping_timer(echo_check);
    }
}

static void sensor_manager::read_all() {
    for (int i = 0; i < n_sensors; i++) {
        read_sensor(i);
    }
}

// filter the sensor values.
void sensor_manager::process() {
    for (int i = 0; i < n_sensors; i++) {
        // filter[i].add_reading(sensors[i]->reading);
        values[i] = filter[i].value();
        // values[i] = sensors[i]->reading;
    }
}

