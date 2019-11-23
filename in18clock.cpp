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
#include "User_Setup.h"
#include "alarm.h"
#include "date.h"
#include "debug.h"
#include "display.h"
#include "ds18b20.h"
#include "leds.h"
#include "menu.h"
#include "onoff.h"
#include "rtc.h"
#include "temperature.h"
#include "time.h"
#include "tone.h"

menu_struct menu[] = {
	/* Parent,	First Child,	Last Child,	Value,		Min,		Max,		EEPROM Offset,	Blink,			Display,	Edit,		Inc.			Dec.			Save 		On Show */
/* Display time */
/*  0 */ {NO_PARENT,	6,	9,	0,	0,	0,	NO_LOAD,	0,		timeDisplay,	timeEdit,	0,	0, 	timeOnSave,	timeOnShow},
/* Display date */
/*  1 */ {NO_PARENT,	10,	13,	0,	0,	0,	NO_LOAD,	0,		dateDisplay,	dateEdit,	0,	0,	dateOnSave,	0},
/* Display temperature */
/*  2 */ {NO_PARENT,	14,	14,	0,	0,	0,	NO_LOAD,	0,		tempDisplay,	0,	0,	0,	tempSave,	0},
/* Display alarm */
/*  3 */ {NO_PARENT,	15,	19,	0,	0,	0,	NO_LOAD,	0,		alarmDisplay,	0,	0,	0,	alarmOnSave,	0},
/* LED configuration */
/*  4 */ {NO_PARENT,	20,	23,	0,	0,	0,	NO_LOAD,	0,		ledsDisplay,	0,	0,	0,	ledsOnSave,	0},
/* Turn On/Off Time */
/*  5 */ {NO_PARENT,	24,	28,	0,	0,	0,	NO_LOAD,	0,		onOffDisplay,	0,	0,	0,	onOffSave,	0},
/* 12/24 hours edit */
/*  6 */ {0,	NO_CHILD,	NO_CHILD,	0,	0,	1,	1,	B00001100,		timeEditDisplay,0,	0,	0,	0,0},
/* Hours edit */
/*  7 */ {0,	NO_CHILD,	NO_CHILD,	0,	0,	23,	NO_LOAD,	B00000011,		timeEditDisplay,0,	0,	0,	0,0},
/* Minutes edit */
/*  8 */ {0,	NO_CHILD,	NO_CHILD,	0,	0,	59,	NO_LOAD,	B00001100,		timeEditDisplay,0,	0,	0,	0,0},
/* Seconds edit */
/*  9 */ {0,	NO_CHILD,	NO_CHILD,	0,	0,	59,	NO_LOAD,	B00110000,		timeEditDisplay,0,	0,	0,	0,0},
/* Date format */
/* 10 */ {1,	NO_CHILD,	NO_CHILD,	0,	0,	1,	13,	B00111111,		dateEditDisplay,0,	0,	0,	0,0},
/* Day edit */
/* 11 */ {1,	NO_CHILD,	NO_CHILD,	0,	1,	31,	NO_LOAD,	B00000011,		dateEditDisplay,0,	0,	0,	0,0},
/* Month edit */
/* 12 */ {1,	NO_CHILD,	NO_CHILD,	0,	1,	12,	NO_LOAD,	B00001100,		dateEditDisplay,0,	0,	0,	0,0},
/* Year edit */
/* 13 */ {1,	NO_CHILD,	NO_CHILD,	0,	0,	99,	NO_LOAD,	B00110000,		dateEditDisplay,0,	0,	0,	0,0},
/* Celsius/Fahrenheit edit */
/* 14 */ {2,	NO_CHILD,	NO_CHILD,	0,	0,	1,	11,	B00001100,		tempEditDisplay,0,	0,	0,	0,0},
/* Alarm Hours edit */
/* 15 */ {3,	NO_CHILD,	NO_CHILD,	0,	0,	23,	2,	B00000011,		alarmDisplay,	0,	0,	0,	0,0},
/* Alarm Minutes edit */
/* 16 */ {3,	NO_CHILD,	NO_CHILD,	0,	0,	59,	3,	B00001100,		alarmDisplay,	0,	0,	0,	0,0},
/* Alarm Seconds edit */
/* 17 */ {3,	NO_CHILD,	NO_CHILD,	0,	0,	59,	4,	B00110000,		alarmDisplay,	0,	0,	0,	0,0},
/* Alarm Enable edit */
/* 18 */ {3,	NO_CHILD,	NO_CHILD,	0,	0,	1,	5,	B00111111,		alarmDisplay,	0,	0,	0,	0,0},
/* Alarm Song */
/* 19 */ {3,	NO_CHILD,	NO_CHILD,	0,	0,	6,	19,	B00001100,		alarmDisplay,	0,	0,	0,	0,0},
/* LED configuration */
/* 20 */ {4,	NO_CHILD,	NO_CHILD,	1,	0,	2,	20,	B00001100,		ledsDisplay,	0,	0,	0,	0,0},
/* LED Red color */
/* 21 */ {4,	NO_CHILD,	NO_CHILD,	0,	0,	63,	8,	B00000011,		ledsDisplay,	0,	0,	0,	0,0},
/* LED Green color */
/* 22 */ {4,	NO_CHILD,	NO_CHILD,	0,	0,	63,	9,	B00001100,		ledsDisplay,	0,	0,	0,	0,0},
/* LED Blue color */
/* 23 */ {4,	NO_CHILD,	NO_CHILD,	0,	0,	63,	10,	B00110000,		ledsDisplay,	0,	0,	0,	0,0},
/* Turn Off Hour */
/* 24 */ {5,	NO_CHILD,	NO_CHILD,	0,	0,	23,	14,	B00001100,		onOffDisplay,	0,	0,	0,	0,0},
/* Turn Off Minute */
/* 25 */ {5,	NO_CHILD,	NO_CHILD,	0,	0,	59,	15,	B00110000,		onOffDisplay,	0,	0,	0,	0,0},
/* Turn On Hour */
/* 26 */ {5,	NO_CHILD,	NO_CHILD,	0,	0,	23,	16,	B00001100,		onOffDisplay,	0,	0,	0,	0,0},
/* Turn On Minute */
/* 27 */ {5,	NO_CHILD,	NO_CHILD,	0,	0,	59,	17,	B00110000,		onOffDisplay,	0,	0,	0,	0,0},
/* Turn On/Off Enable */
/* 28 */ {5,	NO_CHILD,	NO_CHILD,	0,	0,	1,	18,	B00111100,		onOffDisplay,	0,	0,	0,	0,0},
};

void setup() {
	debugSetup();
	displaySetup();
	toneSetup();
	rtcSetup();
	ledsSetup();
	rtcGetTime();
	ds18b20Setup();
	menuSetup(menu, sizeof(menu)/sizeof(menu_struct));
	doTest();
	ledsOff();
	alarmSetup();
}

void loop() {

	if(millis() % 10000 == 0) {
		rtcGetTime();
	}

	ledsUpdate();
	alarmUpdate();
	toneUpdate();
	menuUpdate();
}

void doTest()
{
	String testStringArray[11]={"000000","111111","222222","333333","444444","555555","666666","777777","888888","999999",""};
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