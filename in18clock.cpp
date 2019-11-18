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
#include "date.h"
#include "debug.h"
#include "display.h"
#include "ds18b20.h"
#include "leds.h"
#include "menu.h"
#include "rtc.h"
#include "temperature.h"
#include "time.h"

menu_struct menu[] = {
	/* Parent,	First Child,	Last Child,	Value,		Min,		Max,		EEPROM Offset,	Blink,			Display,	Edit,		Inc.			Dec.			Save 		On Show */
/*  0 */ {NO_PARENT,	3,	6,	0,	0,	0,	NO_LOAD,	0,		timeDisplay,	timeEdit,	0,	0, 	timeOnSave,	timeOnShow},
/*  1 */ {NO_PARENT,	7,	10,	0,	0,	0,	NO_LOAD,	0,		dateDisplay,	dateEdit,	0,	0,	dateOnSave},
/*  2 */ {NO_PARENT,	11,	11,	0,	0,	0,	NO_LOAD,	0,		tempDisplay,	0,	0,	0,	0},
/*  3 */ {0,	NO_CHILD,	NO_CHILD,	0,	0,	1,	1,	B00001100,		timeEditDisplay,0,	0,	0,	0},
/*  4 */ {0,	NO_CHILD,	NO_CHILD,	0,	0,	23,	NO_LOAD,	B00000011,		timeEditDisplay,0,	0,	0,	0},
/*  5 */ {0,	NO_CHILD,	NO_CHILD,	0,	0,	59,	NO_LOAD,	B00001100,		timeEditDisplay,0,	0,	0,	0},
/*  6 */ {0,	NO_CHILD,	NO_CHILD,	0,	0,	59,	NO_LOAD,	B00110000,		timeEditDisplay,0,	0,	0,	0},
/*  7 */ {1,	NO_CHILD,	NO_CHILD,	0,	0,	1,	13,	B00111111,		dateEditDisplay,0,	0,	0,	0},
/*  8 */ {1,	NO_CHILD,	NO_CHILD,	0,	1,	31,	NO_LOAD,	B00000011,		dateEditDisplay,0,	0,	0,	0},
/*  9 */ {1,	NO_CHILD,	NO_CHILD,	0,	1,	12,	NO_LOAD,	B00001100,		dateEditDisplay,0,	0,	0,	0},
/* 10 */ {1,	NO_CHILD,	NO_CHILD,	0,	0,	99,	NO_LOAD,	B00110000,		dateEditDisplay,0,	0,	0,	0},
/* 11 */ {2,	NO_CHILD,	NO_CHILD,	0,	0,	1,	11,	B00001100,		tempEditDisplay,0,	0,	0,	tempSave}
};

void setup() {
	debugSetup();
	displaySetup();
	rtcSetup();
	ledsSetup();
	rtcGetTime();
	ds18b20Setup();
	menuSetup(menu, sizeof(menu)/sizeof(menu_struct));
	doTest();
	ledsOff();
}

void loop() {

	if(millis() % 10000 == 0) {
		rtcGetTime();
	}

	//ledsUpdate();
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