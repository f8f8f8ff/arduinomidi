#ifndef _SENSOR_H
#define _SENSOR_H
#include <MedianFilterLib2.h>
#include <NewPing.h>

#include "midi.h"

#define FILTER_WIDTH 6
#define PING_INTERVAL 30
#define MAX_MIDIVAL 1270 

struct Sensor {
    byte trigPin;
    byte echoPin;
    NewPing sonar;
    unsigned long timer;
    int currentReading;
    int distance;
    int minDistance = 10;
    int maxDistance = 300;
    MedianFilter2<int> readings;

    int cc;
    int midiValue8; // 0-255
    int midiValueInterp;
    int changed;

    Sensor(int cc, byte trigPin, byte echoPin, int min = 0, int max = 300);
};

extern uint8_t NUM_SENSORS;
extern Sensor SENSOR[];

void printSensor(Sensor& s);
void printSensors();

void initSensorTimers();
void readSensor(Sensor& s);
void readSensorNonBlocking(Sensor& s);
void echoCheck();
void readSensors();

// 0 = > max or < min; 127 = min
void interpretSensor(Sensor& s);
void interpretSensors();
void interpolateSensor(Sensor& s);
void interpolateSensors();
void sendSensor(Sensor& s, bool interpolated = true);
void sendMidi();
void touchSensor(Sensor& s);
#endif