arduino midi controller using ultrasonic distance sensors.
made for doug aitken workshop.
uses MIDIUSB library. working on a pro micro now.

accepts some serial commands:
    start
    stop
    state           returns current state
    delay [ms]      prints and optionally sets delay between reads
    read [sensor#]  prints sensor data
    touch sensor#   reads specified sensor and sends midi message
