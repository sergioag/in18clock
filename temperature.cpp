/*
 * Firmware for GRA & AFCH's NCS318 Nixie Clock
 * Copyright 2019 Sergio Aguayo
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this
 *    list of conditions and the following disclaimer in the documentation and/or
 *    other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
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
