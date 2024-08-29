#include <NewPing.h>

struct hcsr04 {
    uint8_t pin_in;
    uint8_t pin_out;
    int reading;
    int current_reading;
    unsigned int timer;
    unsigned int max_distance;
    NewPing np;
    hcsr04(uint8_t pin_in, uint8_t pin_out, unsigned int max_distance_cm = 200)
        : pin_in(pin_in),
          pin_out(pin_out),
          timer(0),
          reading(0),
          current_reading(0),
          max_distance(max_distance_cm),
          np(NewPing(pin_out, pin_in, max_distance)) {}
};
