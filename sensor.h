#pragma once
#include <NewPing.h>

#include "midi.h"
#include "ultrasonic.h"

const int num_readings = 5;

struct Sensor {
    byte trigPin;
    byte echoPin;
    NewPing sonar;
    int rawValue;
    int readings[num_readings];
    int readIndex = 0;
    int readingsTotal;
    int minDistance = 10;
    int maxDistance = 300;
    int midiCC;
    int midiValue;
    int midiValuePrev;
    int midiValueInterp;
    int changed;
    Sensor(byte trigPin, byte echoPin, int midiCC, int min = 0, int max = 300)
        : trigPin(trigPin),
          echoPin(echoPin),
          midiCC(midiCC),
          minDistance(min),
          maxDistance(max),
          sonar(NewPing(trigPin, echoPin, max)) {}
};

const int NUM_SENSORS = 1;
Sensor SENSOR[NUM_SENSORS] = {Sensor(9, 8, 0, 0, 300)};

void printSensor(int i) {
    char buffer[1024];
    Sensor s = SENSOR[i];
    sprintf(buffer,
            "id: %2d raw: %4d min: %4d max %4d val: %3d cc: %2d changed: %d "
            "trig: %2d echo %2d",
            i, s.rawValue, s.minDistance, s.maxDistance, s.midiValue, s.midiCC,
            s.changed, s.trigPin, s.echoPin);
    Serial.println(buffer);
}

void printSensors() {
    for (int i = 0; i < NUM_SENSORS; i++) {
        printSensor(i);
    }
}

void readSensor(int i) {
    //    SENSOR[i].rawValue = ultrasonicRead(SENSOR[i].trigPin,
    //    SENSOR[i].echoPin);
    Sensor& s = SENSOR[i];
    int distance = ultrasonicRead_newping(s.sonar);
    
    s.readingsTotal = s.readingsTotal - s.readings[s.readIndex];
    s.readings[s.readIndex] = distance;
    s.readingsTotal = s.readingsTotal + s.readings[s.readIndex];
    s.readIndex += 1;
    if (s.readIndex >= num_readings) {
        s.readIndex = 0;
    }
    s.rawValue = s.readingsTotal / num_readings;
}

void readSensors() {
    for (int i = 0; i < NUM_SENSORS; i++) {
        readSensor(i);
    }
}

// 0 = > max or < min; 127 = min
void interpretSensor(int i) {
    Sensor& s = SENSOR[i];
    int value = map(s.rawValue, s.maxDistance, s.minDistance, 0, 127);
    if (s.rawValue < s.minDistance) {
        value = 0;
    }
    s.changed = value != s.midiValue;
    s.midiValuePrev = s.midiValue;
    s.midiValue = value;
}

void interpretSensors() {
    for (int i = 0; i < NUM_SENSORS; i++) {
        interpretSensor(i);
    }
}

void interpolateSensor(int i) {
    Sensor& s = SENSOR[i];
    if (s.midiValue > s.midiValueInterp) {
        s.midiValueInterp += 1;
    } else if (s.midiValue < s.midiValueInterp) {
        s.midiValueInterp -= 1;
    }
}

void interpolateSensors() {
    for (int i = 0; i < NUM_SENSORS; i++) {
        interpolateSensor(i);
    }
}

void sendSensor(int i) {
    if (SENSOR[i].changed) {
        controlChange(0, SENSOR[i].midiCC, SENSOR[i].midiValueInterp);
    }
}

void sendMidi() {
    for (int i = 0; i < NUM_SENSORS; i++) {
        sendSensor(i);
    }
	MidiUSB.flush();
}

void touchSensor(int i) {
    readSensor(i);
    interpretSensor(i);
    SENSOR[i].changed = true;  // force send midi message
    sendSensor(i);
    printSensor(i);
    MidiUSB.flush();
}
