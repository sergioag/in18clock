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

boolean dateEdit()
{
	menuSetValue(MENU_EDIT_DAY, day());
	menuSetValue(MENU_EDIT_MONTH, month());
	menuSetValue(MENU_EDIT_YEAR, year() % 100);
	return true;
}

void dateOnSave()
{
	rtc_info *rtcInfo = rtcGetTime();
	rtcInfo->days = menuGetValue(MENU_EDIT_DAY);
	rtcInfo->months = menuGetValue(MENU_EDIT_MONTH);
	rtcInfo->years = ((rtcInfo->years / 100) * 100) +  menuGetValue(MENU_EDIT_YEAR);

	rtcSetTime(rtcInfo);

	menuSave(MENU_EDIT_DATE_FORMAT);
}