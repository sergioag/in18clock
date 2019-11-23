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
#include <Arduino.h>
#include <Time.h>
#include "display.h"
#include "menu.h"
#include "onoff.h"
#include "rtc.h"
#include "time.h"
#include "utils.h"
#include "User_Setup.h"

boolean antiPoisoningInProgress = false;
unsigned long lastDigitChange;
String digitCycle[] = {
	"000000", "111111", "222222", "333333", "444444", "555555", "666666", "777777", "888888", "999999", "000000"
};
#define CYCLE_DELAY		150
#define MAX_DIGITS_CYCLE	11
#define DISPLAY_GRACE_TIME	10000
int currentDigit = 0;
long lastShow = 0;

String getTimeString(int hours, int minutes, int seconds)
{
	switch(menuGetValue(MENU_EDIT_TIME_FORMAT)) {
		case TIME_FORMAT_24H:
			return PreZero(hours) + PreZero(minutes) + PreZero(seconds);
		case TIME_FORMAT_12H:
			if(hours > 12) hours -= 12;
			if(hours == 0) hours = 12;
			return PreZero(hours) + PreZero(minutes) + PreZero(seconds);
	}
}

void resetAntiPoisoning()
{
	lastDigitChange = millis();
	currentDigit = 0;
	antiPoisoningInProgress = true;
}

void timeOnShow(boolean isShowing)
{
	if(isShowing) {
		timeWakeUpDisplay();
	}
	else {
		displayPowerOn();
	}
}

void timeWakeUpDisplay()
{
	lastShow = millis();
}

String antiPoisoning(String str)
{
	if(antiPoisoningInProgress) {
		str = digitCycle[currentDigit];
		if(millis() - lastDigitChange > CYCLE_DELAY) {
			lastDigitChange = millis();
			currentDigit++;
			if(currentDigit == MAX_DIGITS_CYCLE) antiPoisoningInProgress = false;
		}
	}
	return str;
}
void timeDisplay()
{
	if(onOffShouldTurnOff() && millis() - lastShow > DISPLAY_GRACE_TIME) {
		displayPowerOff();
	}
	else {
		String str = getTimeString(hour(), minute(), second());
		if(!antiPoisoningInProgress && second() == 0) {
			resetAntiPoisoning();
		}
		str = antiPoisoning(str);
		displaySetValue(str);
		displaySetLowerDots(second() & 1);
		displaySetUpperDots(second() & 1);
	}
}

void timeEditDisplay()
{
	if(menuGetCurrentPosition() == MENU_EDIT_TIME_FORMAT) {
		switch(menuGetValue(MENU_EDIT_TIME_FORMAT)) {
			case TIME_FORMAT_24H:
				displaySetValue("002400");
				break;
			case TIME_FORMAT_12H:
				displaySetValue("001200");
				break;
		}
		displaySetUpperDots(false);
		displaySetLowerDots(false);
	}
	else {
		displaySetValue(getTimeString(menuGetValue(MENU_EDIT_HOURS), menuGetValue(MENU_EDIT_MINUTES), menuGetValue(MENU_EDIT_SECONDS)));
		displaySetUpperDots(false);
		displaySetLowerDots(menuGetValue(MENU_EDIT_TIME_FORMAT) == TIME_FORMAT_12H && menuGetValue(MENU_EDIT_HOURS) >= 12);
	}
}

boolean timeEdit() {
	menuSetValue(MENU_EDIT_HOURS, hour());
	menuSetValue(MENU_EDIT_MINUTES, minute());
	menuSetValue(MENU_EDIT_SECONDS, second());
	return true;
}

void timeOnSave()
{
	rtc_info *rtcInfo = rtcGetTime();
	rtcInfo->hours = menuGetValue(MENU_EDIT_HOURS);
	rtcInfo->minutes = menuGetValue(MENU_EDIT_MINUTES);
	rtcInfo->seconds = menuGetValue(MENU_EDIT_SECONDS);

	rtcSetTime(rtcInfo);

	menuSave(MENU_EDIT_TIME_FORMAT);

}