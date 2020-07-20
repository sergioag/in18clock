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

/*
 * This is the main entry point of the clock application. It uses the Arduino standard
 * of of setup()/loop().
 *
 * Also here is the menu definition and the test routine.
 *
 * Keep in mind that this file doesn't contain any actual code (except test code). Instead,
 * it just calls to the appropriate functions that do the actual job. The only exception
 * is the test code executed during startup.
 */

#include <Arduino.h>
#include "User_Setup.h"
#include "alarm.h"
#include "date.h"
#include "debug.h"
#include "display.h"
#include "ds18b20.h"
#include "gps.h"
#include "leds.h"
#include "menu.h"
#include "onoff.h"
#include "rtc.h"
#include "temperature.h"
#include "time.h"
#include "tone.h"

// This is the version number. First two digits are always "99" to differentiate from official firmware.
#define VERSION "990103"

/*
 * This is the menu definition. It defines the menu user all over the application. Please see menu.cpp/menu.h
 * for more details on the implementation specifics.
 */
menu_struct menu[] = {
/* Id,				Parent,			First Child,		Last Child,		Value,		Min,		Max,		EEPROM Offset,	Blink,			Display,	Edit,		Inc.			Dec.			Save 		On Show */
/* Display time */
{MENU_TIME,			NO_PARENT,		MENU_EDIT_TIME_FORMAT,	MENU_EDIT_SECONDS,	0,	0,	0,	NO_LOAD,	0,		timeDisplay,	timeEdit,	0,	0, 	timeOnSave,	timeOnShow},
/* Display date */
{MENU_DATE,			NO_PARENT,		MENU_EDIT_DATE_FORMAT,	MENU_EDIT_YEAR,		0,	0,	0,	NO_LOAD,	0,		dateDisplay,	dateEdit,	0,	0,	dateOnSave,	0},
/* Display temperature */
{MENU_TEMPERATURE,		NO_PARENT,		MENU_EDIT_TEMP_UNIT,	MENU_EDIT_TEMP_UNIT,	0,	0,	0,	NO_LOAD,	0,		tempDisplay,	0,	0,	0,	tempSave,	tempOnShow},
/* Display alarm */
{MENU_ALARM,			NO_PARENT,		MENU_EDIT_AL_HOURS,	MENU_EDIT_AL_SONG,	0,	0,	0,	NO_LOAD,	0,		alarmDisplay,	0,	0,	0,	alarmOnSave,	0},
/* LED configuration */
{MENU_LED_CONFIG,		NO_PARENT,		MENU_EDIT_LED_MODE,	MENU_EDIT_LED_BLUE,	0,	0,	0,	NO_LOAD,	0,		ledsDisplay,	0,	0,	0,	ledsOnSave,	0},
/* Turn On/Off Time */
{MENU_ONOFF_CONFIG,		NO_PARENT,		MENU_EDIT_OFF_HOUR,	MENU_EDIT_ONOFF_ENABLE,	0,	0,	0,	NO_LOAD,	0,		onOffDisplay,	0,	0,	0,	onOffSave,	0},
/* 12/24 hours edit */
{MENU_EDIT_TIME_FORMAT,		MENU_TIME,		NO_CHILD,		NO_CHILD,		0,	0,	1,	1,	B00001100,		timeEditDisplay,0,	0,	0,	0,0},
/* UTC Offset Sign */
{MENU_EDIT_UTC_SIGN,		MENU_TIME,		NO_CHILD,		NO_CHILD,		0,	0,	1,	21,B00111100,		timeEditDisplay,0,	0,	0,	0,0},
/* UTC Offset Hours */
{MENU_EDIT_UTC_HOURS,   	MENU_TIME, 		NO_CHILD,		NO_CHILD,		0,	0,	14,	19, B00001100,		timeEditDisplay,0,	0,	0,	0,0},
/* UTC Offset Minutes */
{MENU_EDIT_UTC_MINUTES, 	MENU_TIME, 		NO_CHILD,		NO_CHILD,		0,	0,	3,	20, B00110000,		timeEditDisplay,0,	0,	0,	0,0},
/* Hours edit */
{MENU_EDIT_HOURS,		MENU_TIME,		NO_CHILD,		NO_CHILD,		0,	0,	23,	NO_LOAD,	B00000011,		timeEditDisplay,0,	0,	0,	0,0},
/* Minutes edit */
{MENU_EDIT_MINUTES,		MENU_TIME,		NO_CHILD,		NO_CHILD,		0,	0,	59,	NO_LOAD,	B00001100,		timeEditDisplay,0,	0,	0,	0,0},
/* Seconds edit */
{MENU_EDIT_SECONDS,		MENU_TIME,		NO_CHILD,		NO_CHILD,		0,	0,	59,	NO_LOAD,	B00110000,		timeEditDisplay,0,	0,	0,	0,0},
/* Date format */
{MENU_EDIT_DATE_FORMAT,		MENU_DATE,		NO_CHILD,		NO_CHILD,		0,	0,	1,	13,B00111111,		dateEditDisplay,0,	0,	0,	0,0},
/* Day edit */
{MENU_EDIT_DAY,			MENU_DATE,		NO_CHILD,		NO_CHILD,		0,	1,	31,	NO_LOAD,	B00000011,		dateEditDisplay,0,	0,	0,	0,0},
/* Month edit */
{MENU_EDIT_MONTH,		MENU_DATE,		NO_CHILD,		NO_CHILD,		0,	1,	12,	NO_LOAD,	B00001100,		dateEditDisplay,0,	0,	0,	0,0},
/* Year edit */
{MENU_EDIT_YEAR,		MENU_DATE,		NO_CHILD,		NO_CHILD,		0,	0,	99,	NO_LOAD,	B00110000,		dateEditDisplay,0,	0,	0,	0,0},
/* Celsius/Fahrenheit edit */
{MENU_EDIT_TEMP_UNIT,		MENU_TEMPERATURE,	NO_CHILD,		NO_CHILD,		0,	0,	1,	11,B00001100,		tempEditDisplay,0,	0,	0,	0,0},
/* Alarm Hours edit */
{MENU_EDIT_AL_HOURS,		MENU_ALARM,		NO_CHILD,		NO_CHILD,		0,	0,	23,	2,	B00000011,		alarmDisplay,	0,	0,	0,	0,0},
/* Alarm Minutes edit */
{MENU_EDIT_AL_MINUTES,		MENU_ALARM,		NO_CHILD,		NO_CHILD,		0,	0,	59,	3,	B00001100,		alarmDisplay,	0,	0,	0,	0,0},
/* Alarm Seconds edit */
{MENU_EDIT_AL_SECONDS,		MENU_ALARM,		NO_CHILD,		NO_CHILD,		0,	0,	59,	4,	B00110000,		alarmDisplay,	0,	0,	0,	0,0},
/* Alarm Enable edit */
{MENU_EDIT_AL_ENABLE,		MENU_ALARM,		NO_CHILD,		NO_CHILD,		0,	0,	1,	5,	B00111111,		alarmDisplay,	0,	0,	0,	0,0},
/* Alarm Song */
{MENU_EDIT_AL_SONG,		MENU_ALARM,		NO_CHILD,		NO_CHILD,		0,	0,	6,	19,B00001100,		alarmDisplay,	0,	0,	0,	0,0},
/* LED configuration */
{MENU_EDIT_LED_MODE,		MENU_LED_CONFIG,	NO_CHILD,		NO_CHILD,		1,	0,	2,	20,B00001100,		ledsDisplay,	0,	0,	0,	0,0},
/* LED Red color */
{MENU_EDIT_LED_RED,		MENU_LED_CONFIG,	NO_CHILD,		NO_CHILD,		0,	0,	63,	8,	B00000011,		ledsDisplay,	0,	0,	0,	0,0},
/* LED Green color */
{MENU_EDIT_LED_GREEN,		MENU_LED_CONFIG,	NO_CHILD,		NO_CHILD,		0,	0,	63,	9,	B00001100,		ledsDisplay,	0,	0,	0,	0,0},
/* LED Blue color */
{MENU_EDIT_LED_BLUE,		MENU_LED_CONFIG,	NO_CHILD,		NO_CHILD,		0,	0,	63,	10,B00110000,		ledsDisplay,	0,	0,	0,	0,0},
/* Turn Off Hour */
{MENU_EDIT_OFF_HOUR,		MENU_ONOFF_CONFIG,	NO_CHILD,		NO_CHILD,		0,	0,	23,	14,B00001100,		onOffDisplay,	0,	0,	0,	0,0},
/* Turn Off Minute */
{MENU_EDIT_OFF_MINUTE,		MENU_ONOFF_CONFIG,	NO_CHILD,		NO_CHILD,		0,	0,	59,	15,B00110000,		onOffDisplay,	0,	0,	0,	0,0},
/* Turn On Hour */
{MENU_EDIT_ON_HOUR,		MENU_ONOFF_CONFIG,	NO_CHILD,		NO_CHILD,		0,	0,	23,	16,B00001100,		onOffDisplay,	0,	0,	0,	0,0},
/* Turn On Minute */
{MENU_EDIT_ON_MINUTE,		MENU_ONOFF_CONFIG,	NO_CHILD,		NO_CHILD,		0,	0,	59,	17,B00110000,		onOffDisplay,	0,	0,	0,	0,0},
/* Turn On/Off Enable */
{MENU_EDIT_ONOFF_ENABLE,	MENU_ONOFF_CONFIG,	NO_CHILD,		NO_CHILD,		0,	0,	1,	18,B00111100,		onOffDisplay,	0,	0,	0,	0,0},
};

/**
 * Setup entry point. As per Arduino's documentation, this is called only once during startup.
 * It should be used to configure hardware as needed.
 *
 * We use it to do so, load configuration, plus run the startup test.
 */
void setup() {
	debugSetup();
	displaySetup();
	toneSetup();
	rtcSetup();
	ledsSetup();
	rtcGetTime();
	ds18b20Setup();
	gpsSetup();
	doTest();
	menuSetup(menu, sizeof(menu)/sizeof(menu_struct));
	alarmSetup();
}

/**
 * This is the loop entry point from Arduino. This is called again and again by the Arduino code.
 */
void loop() {

	if(millis() % 10000 == 0) {
		gpsTimeUpdate();
		rtcGetTime();
	}

	gpsUpdate();
	ledsUpdate();
	alarmUpdate();
	toneUpdate();
	menuUpdate();
}

/**
 * This is the display test code. It will do a LED test and cycle all the digits afterwards. Finally, it wil show
 * the firmware version for 2 seconds before returning.
 */
void doTest()
{
	String testStringArray[11]={"000000","111111","222222","333333","444444","555555","666666","777777","888888","999999",VERSION};
	int dlay=500;
	bool test=1;
	byte strIndex=-1;

	ledsTest();

	unsigned long startOfTest=millis()+1000; //disable delaying in first iteration
	while (test)
	{
		if ((millis()-startOfTest)>dlay)
		{
			startOfTest=millis();
			strIndex=strIndex+1;
			if (strIndex==10) dlay=2000;
			if (strIndex>10) { test=false; strIndex=10;}
			displaySetValue(testStringArray[strIndex]);
		}
	}
}