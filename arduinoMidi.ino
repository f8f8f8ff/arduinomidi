#include "sensor.h"

enum state {RUN, PAUSE};
state STATE = RUN;
int DELAY = 10;

void setup() {
  Serial.begin(9600);
  Serial.println("starting");
}

void loop() {
  parseSerial();
  switch (STATE) {
    case RUN:
      readSensors();
      interpretSensors();
      sendMidi();
      delay(DELAY);
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
    printSensor(i);
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
    touchSensor(i);
  }
  Serial.println("unknown command. try 'start' 'stop' 'state' 'read [#]' 'touch #' 'delay [#]'");
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