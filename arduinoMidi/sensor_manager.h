#ifndef sensor_manager_h
#define sensor_manager_h
#include "hcsr04.h"
#include "medianfilter.h"

namespace sensor_manager {
    static const size_t MAX_SENSORS = 8;

    static hcsr04* sensors[MAX_SENSORS];
    static size_t n_sensors = 0;
    static size_t current_us_sensor = 0;
    static unsigned int ping_interval = 100;
    static median_filter filter[MAX_SENSORS];
    static int values[MAX_SENSORS];

    static void echo_check();
    static void read_sensor(size_t n);
    static void read_all();

    int add_sensor(hcsr04* sensor);
    void read();
    int value(size_t n);
    size_t num_sensors();
    void process();
}
#endif