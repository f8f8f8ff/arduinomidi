#pragma once
#include <NewPing.h>

void ultrasonicInitPins(int trig, int echo) {
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
}

// returns distance in cm
// https://projecthub.arduino.cc/Isaac100/getting-started-with-the-hc-sr04-ultrasonic-sensor-7cabe1
int ultrasonicRead(int trig, int echo) {
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);

    int duration = pulseIn(echo, HIGH);
    // speed of sound: 0.0343 c/µs
    int distance = (duration * 0.0343) / 2;
    if (true) {
        Serial.print("duration (us): ");
        Serial.print(duration);
        Serial.print(" distance: ");
        Serial.println(distance);
    }
    return distance;
}

int ultrasonicRead_basic(int trig, int echo) {
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);

    int duration = pulseIn(echo, HIGH);
    // speed of sound: 0.0343 c/µs
    int distance = (duration * 0.0343) / 2;
    if (true) {
        Serial.print("duration (us): ");
        Serial.print(duration);
        Serial.print(" distance: ");
        Serial.println(distance);
    }
    return distance;
}

int ultrasonicRead_newping(NewPing& p) {
    return p.convert_cm(p.ping_median(5));
    // return p.ping_cm();
};