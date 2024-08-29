#include "sensor.h"

#define DEBUG true

Sensor::Sensor(int cc, byte trigPin, byte echoPin, int min = 0, int max = 300)
    : trigPin(trigPin),
      echoPin(echoPin),
      cc(cc),
      minDistance(min),
      maxDistance(max),
      sonar(NewPing(trigPin, echoPin, max)),
      readings(MedianFilter2<int>(FILTER_WIDTH)) {}

void printSensor(Sensor& s) {
    char buffer[512];
    sprintf(buffer,
            "cc: %2d raw: %4d min: %4d max %4d val: %3d changed: %d "
            "trig: %2d echo %2d",
            s.cc, s.distance, s.minDistance, s.maxDistance, s.midiValue8,
            s.changed, s.trigPin, s.echoPin);
    Serial.println(buffer);
}

void printSensors() {
    for (int i = 0; i < NUM_SENSORS; i++) {
        printSensor(SENSOR[i]);
    }
}

void initSensorTimers() {
    // TODO
    SENSOR[0].timer = millis() + 75;
    for (int i = 1; i < NUM_SENSORS; i++) {
        SENSOR[i].timer = SENSOR[i - 1].timer + PING_INTERVAL;
    }
}

uint8_t CURRENT_SENSOR;

void readSensor(Sensor& s) {
    SENSOR[CURRENT_SENSOR].sonar.timer_stop();
    s.distance = s.sonar.ping_cm();
}

void addReading(Sensor& s, int reading) { s.readings.AddValue(reading); }

void echoCheck() {
    Sensor& s = SENSOR[CURRENT_SENSOR];
    if (s.sonar.check_timer()) {
        s.currentReading = s.sonar.ping_result / US_ROUNDTRIP_CM;
    }
}

void oneSensorCycle() {
    for (int i = 0; i < NUM_SENSORS; i++) {
        addReading(SENSOR[NUM_SENSORS], SENSOR[NUM_SENSORS].currentReading);
    }
}

void readSensorNonBlocking(Sensor& s) {
    if (millis() >= s.timer) {
        s.timer = s.timer + (PING_INTERVAL * NUM_SENSORS);
        SENSOR[CURRENT_SENSOR].sonar.timer_stop();
        // Serial.println(SENSOR[CURRENT_SENSOR].sonar.ping_result);
        addReading(SENSOR[CURRENT_SENSOR],
                   SENSOR[CURRENT_SENSOR].currentReading);
        CURRENT_SENSOR = s.cc;
        s.currentReading = s.maxDistance+1;
        s.sonar.ping_timer(echoCheck);
    }
}

void readSensors() {
    for (int i = 0; i < NUM_SENSORS; i++) {
        readSensorNonBlocking(SENSOR[i]);
    }
}

// 0 = > max or < min; 255 = min
void interpretSensor(Sensor& s) {
    s.distance = s.readings.GetFiltered();
    // if (s.distance == 1) {
    //     s.distance = 0;
    // }
    if (false) {
        Serial.print("cc: ");
        Serial.print(s.cc);
        Serial.print(" distance:");
        Serial.println(s.distance);
    }
    int value = map(s.distance, s.maxDistance, s.minDistance, 0, MAX_MIDIVAL);
    if (s.distance < s.minDistance) { 
        Serial.println("distance < minDistance");
        value = MAX_MIDIVAL;
    }
    if (value > MAX_MIDIVAL) value = MAX_MIDIVAL;
    if (value < 0) value = 0;
    s.midiValue8 = value;
}

void interpretSensors() {
    for (int i = 0; i < NUM_SENSORS; i++) {
        interpretSensor(SENSOR[i]);
    }
}

void interpolateSensor(Sensor& s) {
    s.changed = true;
    if (s.midiValue8 > s.midiValueInterp) {
        s.midiValueInterp += 10; // 1 = 0.1 midi value
    } else if (s.midiValue8 < s.midiValueInterp) {
        s.midiValueInterp -= 2;
    } else {
        s.changed = false;
    }
    if (s.midiValueInterp > MAX_MIDIVAL) s.midiValueInterp = MAX_MIDIVAL;
    if (s.midiValueInterp < 0) s.midiValueInterp = 0;
}

void interpolateSensors() {
    for (int i = 0; i < NUM_SENSORS; i++) {
        interpolateSensor(SENSOR[i]);
    }
}

void sendSensor(Sensor& s, bool interpolated = true) {
    interpretSensor(s);
    int val = s.midiValue8;
    if (interpolated) {
        interpolateSensor(s);
        val = s.midiValueInterp;
    }
    val = val/(MAX_MIDIVAL/127); // convert to 0-127;
    if (s.changed) {
        controlChange(0, s.cc, val);
    }
    if (DEBUG) {
        Serial.print("sendSensor cc: ");
        Serial.print(s.cc);
        Serial.print(" val: ");
        Serial.print(val);
        Serial.print(" distance: ");
        Serial.print(s.distance);
        Serial.print("\n");
    }
}

void sendMidi() {
    for (int i = 0; i < NUM_SENSORS; i++) {
        sendSensor(SENSOR[i]);
    }
    MidiUSB.flush();
}

void touchSensor(Sensor& s) {
    readSensor(s);
    s.changed = true;  // force send midi message
    sendSensor(s, false);
    printSensor(s);
    MidiUSB.flush();
}