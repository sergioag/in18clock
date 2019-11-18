#include <Arduino.h>
#include "debug.h"

void debugSetup()
{
	Serial.begin(115200);
}

void debugOutput(String str)
{
	Serial.println(str);
}

void debugOutput(String str, int value)
{
	Serial.print(str);
	Serial.println(value);
}