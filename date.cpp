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
#include "debug.h"
#include "display.h"
#include "menu.h"
#include "rtc.h"
#include "date.h"
#include "utils.h"
#include "User_Setup.h"

String getDateString(int day, int month, int year)
{
	switch(menuGetValue(MENU_EDIT_DATE_FORMAT)) {
		case DATE_FORMAT_DDMMYY:
			return PreZero(day) + PreZero(month) + PreZero(year);
		case DATE_FORMAT_MMDDYY:
			return PreZero(month) + PreZero(day) + PreZero(year);
	}
}

void dateDisplay()
{
	displaySetValue(getDateString(day(), month(), year() % 100));
	displaySetLowerDots(true);
	displaySetUpperDots(false);
}

void dateEditDisplay()
{
	if(menuGetCurrentPosition() == MENU_EDIT_DATE_FORMAT) {
		switch(menuGetValue(MENU_EDIT_DATE_FORMAT)) {
			case DATE_FORMAT_DDMMYY:
				displaySetValue("311299");
				menuSetBlinkPattern(MENU_EDIT_DAY, B00000011);
				menuSetBlinkPattern(MENU_EDIT_MONTH, B00001100);
				break;
			case DATE_FORMAT_MMDDYY:
				displaySetValue("123199");
				menuSetBlinkPattern(MENU_EDIT_DAY, B00001100);
				menuSetBlinkPattern(MENU_EDIT_MONTH, B00000011);
				break;
		}
	}
	else {
		String date = getDateString(menuGetValue(MENU_EDIT_DAY),
			      menuGetValue(MENU_EDIT_MONTH),
			      menuGetValue(MENU_EDIT_YEAR));

		displaySetValue(date);
		displaySetLowerDots(true);
		displaySetUpperDots(false);

		debugOutput(date);
	}
}

bool dateEdit()
{
	menuSetValue(MENU_EDIT_DAY, day());
	menuSetValue(MENU_EDIT_MONTH, month());
	menuSetValue(MENU_EDIT_YEAR, year() % 100);
	return true;
}

bool isValidDate()
{
	int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	if (menuGetValue(MENU_EDIT_YEAR) % 4 == 0) days[1] = 29;
	return menuGetValue(MENU_EDIT_DAY) <= days[menuGetValue(MENU_EDIT_MONTH) - 1];

}

void dateOnSave()
{
	if(!isValidDate()) {
		menuSetPosition(MENU_EDIT_DAY);
		return;
	}

	rtc_info *rtcInfo = rtcGetTime();
	rtcInfo->days = menuGetValue(MENU_EDIT_DAY);
	rtcInfo->months = menuGetValue(MENU_EDIT_MONTH);
	rtcInfo->years = ((rtcInfo->years / 100) * 100) +  menuGetValue(MENU_EDIT_YEAR);

	rtcSetTime(rtcInfo);

	menuSave(MENU_EDIT_DATE_FORMAT);
}