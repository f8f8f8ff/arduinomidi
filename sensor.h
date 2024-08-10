#pragma once
#include "midi.h"

struct Sensor {
  byte pin;
  int rawValue;
  int midiCC;
  int midiValue;
  int changed;
  Sensor(byte pin, int midiCC) : pin(pin), midiCC(midiCC){}
};

const int NUM_SENSORS = 1;
Sensor SENSOR[NUM_SENSORS] = {
  Sensor(A0, 0)
};

void printSensor(int i) {
  char buffer[1024];
  Sensor s = SENSOR[i];
  sprintf(buffer, "id: %d\traw: %4d\tval: %3d\tcc: %2d\tchanged: %d", i, s.rawValue, s.midiValue, s.midiCC, s.changed);
  Serial.println(buffer);
}

void printSensors() {
    for (int i = 0; i < NUM_SENSORS; i++) {
        printSensor(i);
    }
}

void readSensor(int i) {
  SENSOR[i].rawValue = analogRead(SENSOR[i].pin);
}

void readSensors() {
  for (int i = 0; i < NUM_SENSORS; i++) {
    readSensor(i);
  }
}

void interpretSensor(int i) {
  int value = map(SENSOR[i].rawValue, 0, 1023, 0, 127);
  SENSOR[i].changed = value != SENSOR[i].midiValue;
  SENSOR[i].midiValue = value;
}

void interpretSensors() {
  for (int i = 0; i < NUM_SENSORS; i++) {
    interpretSensor(i);
  }
}

void sendSensor(int i) {
  if (SENSOR[i].changed) {
    controlChange(0, SENSOR[i].midiCC, SENSOR[i].midiValue);
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
  SENSOR[i].changed = true; // force send midi message
  sendSensor(i);
  printSensor(i);
  MidiUSB.flush();
}

