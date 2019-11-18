This repository contains the source code to an alternative firmware for [GRA & AFCH's NCS318 Nixie clock](http://gra-afch.com/catalog/nixie-tubes-clocks/nixie-tubes-clock-in-acrylic-case-arduino-shield-ncs318-in-18-options-columns-tubes-arduino-gps-temp-sensor-remote-tubes-optional/).
Please note that this clock comes with either the Arduino Uno or Arduino Mega depending on the selected options. For this
alternate firmware, only Arduino Mega is supported. Support for Arduino Uno may be added at some point, but isn't currently
planned.

How to build
============
Even though this uses Arduino libraries, we're not using the Arduino IDE for building. Instead you need:
- CMake 2.8.4 or above
- Arduino IDE installed (tested with 1.8.10, other versions may work)

Running: `cmake .` should configure everything. Afterwards, running `make` is enough for building. If you wish to upload,
`make upload` should take care of it as long as `${PROJECT_NAME}_PORT` is set correctly in CMakeLists.txt as needed for your
environment. Remember that any changes you make in CMakeLists.txt require running cmake again.

Please also note that building has only been tested in macOS Catalina, though I don't see a reason why it wouldn't work in
other systems, including Windows.

Supported Functionality
=======================
This firmware doesn't yet support all the features of the official firmware. Currently supported are:
- Time display
- Time edit
- 12/24 hour format
- Date display
- Date edit
- DDMMYY/MMDDYY date format
- Temperature sensor
- Temperature in Celsius or Fahrenheit
- Anti-poisoning slot-effect each minute
- "Fireworks" effect for LEDs (though it's currently commented out)

Currently unsupported:
- GPS
- Alarm
- LED configuration
- Songs
- IR control

Planned features:
- Turn on/off time, so the clock will turn off and back on at the set times. This is done to avoid using the tubes at times
  you don't care (i.e. while sleeping or being out of the house/office) or because some people find difficult sleeping with
  all the light emitted by the clock.
- Extended IR control support: same support as official version, plus some other keys that may be useful.

How To Use
==========
To be written...
