#include "sensor.h"

#define PING_INTERVAL 30
Sensor SENSOR[] = {
    Sensor(0, 9, 8, 20, 220),
    Sensor(1, 6, 7, 20, 220),
    Sensor(2, 4, 5, 20, 220),
    Sensor(3, 16, 10, 20, 220),
};
uint8_t NUM_SENSORS = 4;

enum state { RUN, PAUSE };
state STATE = RUN;
int DELAY = 30;
int MIDI_INTERVAL = 10;

unsigned long midiTimer = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("starting");
    midiTimer = millis() + 100;
    initSensorTimers();
}

void loop() {
    parseSerial();
    switch (STATE) {
        case RUN:
            readSensors();
            if (millis() >= midiTimer) {
                midiTimer = millis() + MIDI_INTERVAL;
                sendMidi();
            }
            break;
        case PAUSE:
            break;
    }
}

void parseSerial() {
    if (Serial.available() == 0) {
        return;
    }
    String str = Serial.readString();
    str.trim();
    str.toLowerCase();

    if (str.equals("start")) {
        STATE = RUN;
        init();
        printState();
        return;
    }
    if (str.equals("stop")) {
        STATE = PAUSE;
        printState();
        return;
    }
    if (str.equals("state")) {
        printState();
        return;
    }
    if (str.startsWith("delay")) {
        int i = parseCommandNumber(&str, "delay");
        if (i >= 0) {
            DELAY = i;
        }
        Serial.print("delay (ms): ");
        Serial.println(DELAY);
        return;
    }
    if (str.startsWith("read")) {
        int i = parseCommandNumber(&str, "read");
        if (!sensorNumberOK(i)) {
            Serial.println("bad number, reading all");
            printSensors();
            return;
        }
        printSensor(SENSOR[i]);
        return;
    }
    if (str.startsWith("touch")) {
        if (STATE == RUN) {
            Serial.println("consider pausing before touching sensor");
        }
        int i = parseCommandNumber(&str, "touch");
        if (!sensorNumberOK) {
            Serial.print("sensor number out of range: ");
            Serial.println(i);
            return;
        }
        Serial.print("touched sensor ");
        Serial.println(i);
        touchSensor(SENSOR[i]);
        return;
    }
    Serial.println(
        "unknown command. try 'start' 'stop' 'state' 'read [#]' 'touch #' "
        "'delay [#]'");
}

bool sensorNumberOK(int i) {
    if (i < 0 || i >= NUM_SENSORS) {
        return false;
    }
    return true;
}

int parseCommandNumber(String *str, const char *command) {
    if ((*str).length() <= sizeof("read")) {
        return -1;
    }
    (*str).remove(0, sizeof("read"));
    if ((*str).length() < 1) {
        return -1;
    }
    return (*str).toInt();
}

void printState() {
    Serial.print("STATE: ");
    if (STATE == RUN) {
        Serial.println("RUN");
    } else if (STATE == PAUSE) {
        Serial.println("PAUSE");
    }
}