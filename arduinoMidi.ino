#include <MIDIUSB.h>
#include <MIDIUSB_Defs.h>
#include <frequencyToNote.h>
#include <pitchToFrequency.h>
#include <pitchToNote.h>

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void setup() {
  Serial.begin(9600);
  Serial.println("starting");
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

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

enum state {RUN, PAUSE};
state STATE = RUN;

void loop() {
  readSerial();
  switch (STATE) {
    case RUN:
      readSensors();
      interpretSensors();
      sendMidi();
      delay(10);
      break;
    case PAUSE:
      break;
  }
}

void readSerial() {
  if (Serial.available() == 0) {
    return;
  }
  String str = Serial.readString();
  str.trim();
  str.toLowerCase();
  if (str.equals("start")) {
    STATE = RUN;
    printState();
  } else if (str.equals("stop")) {
    STATE = PAUSE;
    printState();
  } else if (str.equals("state")) {
    printState();
  } else if (str.startsWith("read ")) {
    int i = parseCommandNumber(&str, "read");
    if (i == -1) {
      Serial.println("bad command");
      return;
    }
    printSensor(i);
  } else if (str.startsWith("touch ")) {
    int i = parseCommandNumber(&str, "touch");
    if (i == -1) {
      Serial.println("bad command");
      return;
    }
    Serial.print("touched sensor ");
    Serial.println(i);
    touchSensor(i);
  } else {
    Serial.println("i don't know what you mean.");
  }
}

int parseCommandNumber(String *str, const char *command) {
  if ((*str).length() <= sizeof("read")) {
    return -1;
  }
  (*str).remove(0, sizeof("read"));
  int i = (*str).toInt();
  if (i < 0 || i >= NUM_SENSORS) {
    return -1;
  }
  return i;
}

void printState() {
  Serial.print("STATE: ");
  if (STATE == RUN) {
    Serial.println("RUN");
  } else if (STATE == PAUSE) {
    Serial.println("PAUSE");
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

void printSensor(int i) {
  char buffer[1024];
  Sensor s = SENSOR[i];
  sprintf(buffer, "id: %d\traw: %4d\tval: %3d\tcc: %2d\tchanged: %d", i, s.rawValue, s.midiValue, s.midiCC, s.changed);
  Serial.println(buffer);
}