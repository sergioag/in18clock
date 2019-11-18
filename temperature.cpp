#include <OneWire.h>
#include "display.h"
#include "ds18b20.h"
#include "menu.h"
#include "temperature.h"
#include "utils.h"
#include "User_Setup.h"

void tempDisplay()
{
	String temp = "000000";

	if(ds18b20IsPresent()) {
		float celsius = ds18b20ReadTemperature();
		float temperature;
		if(menuGetValue(MENU_EDIT_TEMP_UNIT) == TEMP_FAHRENHEIT) {
			temperature = (celsius * 1.8 + 32.0) * 100;
		}
		else {
			temperature = celsius * 100;
		}

		int iTemperature = round(temperature);
		if (abs(iTemperature) < 10) temp = "00000" + String(abs(iTemperature));
		if (abs(iTemperature) < 100) temp = "0000" + String(abs(iTemperature));
		if (abs(iTemperature) < 1000) temp = "000" + String(abs(iTemperature));
		if (abs(iTemperature) < 10000) temp = "00" + String(abs(iTemperature));
		if (abs(iTemperature) < 100000) temp = "0" + String(abs(iTemperature));

		displaySetLowerDots(true);
		displaySetUpperDots(iTemperature < 0);
	}

	displaySetValue(temp);
}

void tempEditDisplay()
{
	displaySetValue("00" + PreZero(menuGetValue(MENU_EDIT_TEMP_UNIT)) + "00");
	displaySetLowerDots(false);
	displaySetUpperDots(false);
}

void tempSave()
{
	menuSave(MENU_EDIT_TEMP_UNIT);
}
