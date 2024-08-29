#ifndef midi_mixer_h
#define midi_mixer_h
#include "midi.h"

static const bool MIDI_DEBUG = true;

static const size_t MAX_DATA_CHANNELS = 16;
static const size_t MAX_MIDI_FUNC = 4;
const int MIDI_MULTIPLIER = 16;
const int MAX_MIDI = 127*MIDI_MULTIPLIER;

enum midi_type : byte {
    MIDI_NOTE_ON,
    MIDI_NOTE_OFF,
    MIDI_CC
};

struct midi_message {
    midi_type type;
    byte channel;
    byte control;   // pitch
    byte value;     // velocity
};

typedef midi_message(*midi_func)(int* data, size_t n_data);

class midi_mixer {
    unsigned int timer_ms;
    size_t n_data;
    int value_raw[MAX_DATA_CHANNELS]; // raw data from sensors
    int value_setpoint[MAX_DATA_CHANNELS];
    int value_midi[MAX_DATA_CHANNELS]; // midi values multiplied by MIDI_MULTIPLIER;
    bool changed[MAX_DATA_CHANNELS];
    byte attack[MAX_DATA_CHANNELS]; // 10 = 1 / second
    byte release[MAX_DATA_CHANNELS]; // 10 = 1 / second
    int16_t min[MAX_DATA_CHANNELS]; 
    int16_t max[MAX_DATA_CHANNELS];
    
public:
    unsigned int interval_ms;
    bool check_timer();
    void input(int data, size_t c);
    int map_raw(int raw, int min, int max);
    void interpolate();
    void mix();
    void flush();
    void set_channel_range(size_t c, int16_t min, int16_t max);

    midi_mixer() : n_data(0), interval_ms(100), timer_ms(interval_ms) {
        for (int i = 0; i < MAX_DATA_CHANNELS; i++) {
            value_raw[i] = 0;
            value_setpoint[i] = 0;
            value_midi[i] = 0;
            changed[i] = false;
            attack[i] = 10;
            release[i] = 2;
            min[i] = 10;
            max[i] = 200;
        }
    }
};

bool midi_mixer::check_timer() {
    unsigned int time = millis();
    if (false) {
        Serial.print("midi_mixer::check_timer() ");
        Serial.print("time ");
        Serial.print(time);
        Serial.print(" timer_ms ");
        Serial.print(timer_ms);
        Serial.print("\n");
    }
    if (time >= timer_ms) {
        timer_ms += interval_ms;
        return true;
    }
    return false;
}

void midi_mixer::input(int data, size_t channel) {
    if (channel >= MAX_DATA_CHANNELS) return;
    if (n_data == 0) n_data = 1;
    if (channel > n_data-1) n_data = channel-1;
    value_raw[channel] = data;
    value_setpoint[channel] = map_raw(value_raw[channel], min[channel], max[channel]);
    if (true) {
        Serial.print("input");
        Serial.print(" data ");
        Serial.print(data);
        Serial.print(" setpoint ");
        Serial.print(value_setpoint[channel]);
        Serial.print("\n");
    }
}

int midi_mixer::map_raw(int raw, int min, int max) {
    if (raw < min) {
        return MAX_MIDI;
    }
    int value = map(raw, max, min, 0, MAX_MIDI);
    if (false) {
        Serial.print("map_raw");
        Serial.print(" raw ");
        Serial.print(raw);
        Serial.print(" value ");
        Serial.print(value);
        Serial.print("\n");
    }
    if (value > MAX_MIDI) return MAX_MIDI;
    if (value < 0) return 0;
    return value;
}

void midi_mixer::interpolate() {
    for (int i = 0; i < n_data; i++) {
        changed[i] = true;
        if (value_setpoint[i] > value_midi[i]) {
            value_midi[i] += attack[i];
        } else if (value_setpoint[i] < value_midi[i]) {
            value_midi[i] -= release[i];
        } else {
            changed[i] = false;
        }
        if (value_midi[i] > MAX_MIDI) {
            value_midi[i] = MAX_MIDI;
        } else if (value_midi[i] < 0) {
            value_midi[i] = 0;
        }
        if (false) {
          Serial.print("interp ");
          Serial.print(i);
          Serial.print(" raw ");
          Serial.print(value_raw[i]);
          Serial.print(" setpoint ");
          Serial.print(value_setpoint[i]);
          Serial.print(" value ");
          Serial.print(value_midi[i]);
          Serial.print("\n");
        }
    }
}

void midi_mixer::mix() {
    for (int i = 0; i < n_data; i++) {
        controlChange(0, i, value_midi[i]/MIDI_MULTIPLIER);
    }
}

void midi_mixer::flush() {
    MidiUSB.flush();
}

void midi_mixer::set_channel_range(size_t channel, int16_t min_, int16_t max_) {
    if (channel >= MAX_DATA_CHANNELS) return;
    min[channel] = min_;
    max[channel] = max_;
}
#endif