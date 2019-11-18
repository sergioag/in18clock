#ifndef IN18CLOCK_DISPLAY_H
#define IN18CLOCK_DISPLAY_H

void displaySetup();
void displayPowerOff();
void displayPowerOn();
void displaySetUpperDots(boolean status);
void displaySetLowerDots(boolean status);
void displaySetBlinkMask(byte newBlinkMask);
void displaySetValue(String value);

#endif //IN18CLOCK_DISPLAY_H
