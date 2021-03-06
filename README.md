This repository contains the source code to an alternative firmware for [GRA & AFCH's NCS318 Nixie clock](http://gra-afch.com/catalog/nixie-tubes-clocks/nixie-tubes-clock-in-acrylic-case-arduino-shield-ncs318-in-18-options-columns-tubes-arduino-gps-temp-sensor-remote-tubes-optional/).
Please note that this clock comes with either the Arduino Uno or Arduino Mega depending on the selected options. For this alternate firmware, only Arduino Mega is supported. Support for Arduino Uno may be added at some point, but isn't currently planned.

*Note*: this firmware is not related nor supported by GRA & AFCH. If you want the official firmware, you can find it [here](https://github.com/afch/NixeTubesShieldNCS318).

License
=======
This code is licensed under a 3-clause BSD license, which roughly means that any usage is allowed as long as the license and copyright notice are preserved. Please see [here](https://github.com/sergioag/in18clock/blob/master/LICENSE) for details.

How to build
============
Even though this uses Arduino libraries, we're not using the Arduino IDE for building. Instead you need:
- CMake 2.8.4 or above
- Arduino IDE installed (tested with 1.8.10, other versions may work)

Running: `cmake .` should configure everything. Afterwards, running `make` is enough for building. If you wish to upload, `make upload` should take care of it as long as `${PROJECT_NAME}_PORT` is set correctly in CMakeLists.txt as needed for your environment. Remember that any changes you make in CMakeLists.txt require running cmake again.

Please also note that building has only been tested in macOS Catalina, though I don't see a reason why it wouldn't work in other systems, including Windows.

Supported Functionality
=======================
This firmware supports most of the features of the official firmware, plus some new options. The current list is:
- Time display
- Time edit
- 12/24 hour format
- Date display
- Date edit
- DDMMYY/MMDDYY date format
- Temperature sensor
- Temperature in Celsius or Fahrenheit
- Anti-poisoning slot-effect each minute
- Alert with selectable songs
- LED configuration, with 3 different settings: off, fireworks and fixed (you can configure the specific color from a palette of 262144).
- Turn on/off time: allows selected a time range when your clock is completely powered down. This is done to avoid using the tubes at times you don't care (i.e. while sleeping or being out of the house/office) or because some people find difficult sleeping with all the light emitted by the clock.
- IR control, supporting Sony RM-X153 remotes (only uses Mode, Up and Down keys).
- GPS, including configurable time zone.

Planned features:
- Extended IR control support: use some other keys that may be useful for easier navigation (maybe?).

How To Use
==========
Navigating the menu follows a very simple pattern and is mostly the same as the official firmware, though the menu options themselves are sometimes different.
- Press "Mode" for a short time to swich between menus.
- When you reach the desired menu, hold "Mode" to enter that menu.
- Within any menu, the option you're editting usually is flashing. You can change the value using "Up" and "Down". When you're done changing the value, press "Mode" to go to the next option and repeat. If there is no next option, it will go back to the parent menu.
- If you leave the menu unattended, it will go back one option each 10 seconds. For example, if you're editing the alarm hour time and don't touch the clock for 10 seconds, it will return to the alarm settings menu. After further 10 seconds, it will go back to the time display.

Root Menu
=========
By default, the clock is in the "Time" mode which, you guessed it, displays the time. Pressing mode will cycle between the following modes:
- Time
- Date
- Temperature
- Alarm
- LED configuration
- Turn On/Off time

Time
====
This mode allows viewing and editing the timezone, time and time format. When entering the menu, the following options will be available:
- 12/24 hours format: you can select either "12" or "24" (default).
- Timezone sign: plus (+) is indicated by dots turned off, while minus (-) is indicated with dots turned on.
- Timezone hours (0-14)
- Timezone minutes (0,15,30,45)
- Hours (0-23 or 1-12, depending on the previous setting).
- Minutes (0-59)
- Seconds (0-59)

When 12 hours mode is enabled, the lower dots indicate PM if illuminated or AM if not. The upper dots flash once per second.
When 24 hours mode is enabled, both the lower and upper dots flash once per second.

For example, UTC-8:00 is shown as "000800" with dots turned on. UTC+2 is shown as "000200" with dots turned off. UTC-3:15 is
shown as "000315" with dots turned on.

Date
====
This mode allows viewing and editing the date and date format. When entering the menu, the following options will be available:
- Date format: you can select between DD/MM/YY (default, shown as "31/12/99") or MM/DD/YY (shown as "12/31/99").
- Day (1-31)
- Month (1-12)
- Year (0-99, which is internally adjusted to 2000 to 2099)

Please note that the order in which the date is entered will be the same regardless of the date format, but the position where the day and month are shown will vary accordingly. In any case the value you're changing will be flashing.

Temperature
===========
This mode allows showing the temperature and selecting the units (Celsius or Fahrenheit). The following option is available:
- Temperature Unit: You can choose betwen Celsius (default, shown as "0"), or Fahrenheit (shown as "1").

This mode depends on the temperature accesory being present and correctly connected. Otherwise it will show only zeroes. The temperature is shown with 2 decimals.

Alarm
=====
This allows setting alarm-related settings. This includes:
- Hours (0-23 or 01-12, depending on configuration)
- Minutes (0-59)
- Seconds (0-59)
- Enable/disable: all the tubes will flash. If the upper dots are illuminated, the alarm is enabled.
- Song (0-6): Select the song to play when the alarm activates.

The songs available are:

| Number | Name                          |
| ------ | ----------------------------- |
| 0      | Mission Impossible            |
| 1      | Pink Panther                  |
| 2      | Vanessa Mae?                  |
| 3      | Das Boot                      |
| 4      | Scatman?                      |
| 5      | PopCorn                       |
| 6      | We wish you a Merry Christmas |

When the alarm is sounding, pressing any key will stop it. If the clock is powered down because of the Turn Off time, the alarm will turn it on while it sounds and for a few seconds after finishing.

LED Configuration
=================
This allows configuring the LEDs that illuminate the nixie tubes from below. The following options are available:
- LED mode: Always Off (shown as "0"), Fireworks (default, shown as "1") and Fixed Color (shown as "2")
- Red Color (0-63)
- Green Color (0-63)
- Blue Color (0-63)

Fireworks mode is a smooth transition between lots of different colors, which keeps varying. Fixed Color shows the color you configure. Otherwise, it is ignored.

If you want to see in realtime the result when configuring the Red, Green or Blue color, make sure the LED mode is set to "2" (Fixed Color).

The LEDs are designed to work together with the display. So if you have a turn off time set, the LEDs will be turned off together with the display.

Turn On/Off Time
================
This mode allows configuring the turn off and turn on time, if desired. The following options are available:
- Turn Off Hour
- Turn Off Minute
- Turn On Hour
- Turn On Minute
- Turn On/Off Enable

Throughout this mode, the dots will be illuminated to indicate that the turn on/off time is enabled (which means it will turn off as per the configuration).

When enabled, this means that the clock will turn the LEDs and tubes off starting at the turn off time. It will be kept powered down until the turn on time.

However, there are some exceptions:
- If you press "Mode", the display (and LEDs) will turn on, allowing you to go through the menu and change settings. After going back to the time display, the clock will wait for 10 seconds before turning off again.
- If the alarm sounds, the display (and LEDs) will turn on and kept turned on while the alarm is sounding. After the alarm finishes or it is disabled manually, the clock will wait for 10 seconds before turning off again.
- When you initially plug in your clock, the power on test will execute normally. That is, the tubes and LEDs will turn on. After that step is finished, it will power off according to the configuration.
- Power off doesn't apply to other modes outside of Time display. This is by design because the clock will return to that mode if left unattended and therefore power off shortly afterwards.

New display driver
==================
The default driver will work with hardware v1.1 out of the box. However, it needs rapidly pulsing the tubes because of a low value anode resistor on the tubes. This may reduce the lifespan of the
tubes. If there is no pulsing, the small value of the resistors will cause too much current to flow through the tubes, burning them even faster.

The experimental solution for this involves changing those resistors (labeled R1 through R6) to 14K. Any 1206 resistor should do. After you do this, you must select the new display driver by setting NEW_DISPLAY
to ON in CMakeLists.txt. Keep in mind that this modification is not supported or endorsed by Gra & Afch and will likely void your warranty. Also, I cannot ensure it will actually work correctly.

WARNING: Do not use the new driver unless you've made the hardware modification described or you'll quickly burn the tubes!!!!


Known Issues
============
No issues are currently known.

Third-Party Libraries
=====================
This repository includes the following third-party libraries, found in the "libs" directory:

| Library           | Version    | License  | URL                                              |
| ----------------- | ---------- | -------- | ------------------------------------------------ |
| Adafruit_NeoPixel | 1.3.3      | LGPLv3   | (https://github.com/adafruit/Adafruit_NeoPixel)  |
| ClickButton       | 2013.08.29 | GPLv3    | (https://code.google.com/archive/p/clickbutton/) |
| OneWire           | 2.3.5      | BSD-like | (https://github.com/PaulStoffregen/OneWire)      |
| Time              | 1.5        | LGPLv2.1 | (http://playground.arduino.cc/code/time)         |
| Tone              | 1.0.0      | GPLv3    | (https://github.com/bhagman/Tone)                |
| IRremote          | 2.1.0      | LGPLv2.1 | (https://github.com/shirriff/Arduino-IRremote)   |
| NeoGPS            | 4.2.9      | GPLv3    | (https://github.com/SlashDevin/NeoGPS)           |
| DS1307RTC         | 1.4.1      | LGPLv2.1 | (https://github.com/PaulStoffregen/DS1307RTC)    |
