```
arduino midi controller using ultrasonic distance sensors.
made for DAW.
uses MIDIUSB library. working on a pro micro now.

accepts some serial commands:
    start
    stop
    state           returns current state
    delay [ms]      prints and optionally sets delay between reads
    read [sensor#]  prints sensor data
    touch sensor#   reads specified sensor and sends midi message
```

to build/upload the code, you need the [Arduino IDE](https://www.arduino.cc/en/software).
you may need to download a few libraries through the Arduino IDE, namely:

- MIDIUSB (allows the arduino to act as a MIDI controller)
- NewPing (handles reading the ultrasonic sensors)
- MedianFilterLib2 (filters the ultrasonic sensor data)
