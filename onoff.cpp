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
#include "User_Setup.h"
#include "display.h"
#include "menu.h"
#include "onoff.h"
#include "utils.h"

void onOffDisplay()
{
	switch(menuGetCurrentPosition()) {
		case MENU_EDIT_OFF_HOUR:
		case MENU_EDIT_OFF_MINUTE:
		case MENU_EDIT_ONOFF_ENABLE:
		default:
			displaySetValue("00" + PreZero(menuGetValue(MENU_EDIT_OFF_HOUR)) + PreZero(menuGetValue(MENU_EDIT_OFF_MINUTE)));
			break;
		case MENU_EDIT_ON_HOUR:
		case MENU_EDIT_ON_MINUTE:
			displaySetValue("00" + PreZero(menuGetValue(MENU_EDIT_ON_HOUR)) + PreZero(menuGetValue(MENU_EDIT_ON_MINUTE)));
			break;
	}
	if(menuGetValue(MENU_EDIT_ONOFF_ENABLE)) {
		displaySetUpperDots(true);
		displaySetLowerDots(true);
	}
	else {
		displaySetUpperDots(false);
		displaySetLowerDots(false);
	}
}

void onOffSave()
{
	menuSave(MENU_EDIT_OFF_HOUR);
	menuSave(MENU_EDIT_OFF_MINUTE);
	menuSave(MENU_EDIT_ON_HOUR);
	menuSave(MENU_EDIT_ON_MINUTE);
	menuSave(MENU_EDIT_ONOFF_ENABLE);
}

bool onOffShouldTurnOff()
{
	if(menuGetValue(MENU_EDIT_ONOFF_ENABLE)) {
		int timeOff = (menuGetValue(MENU_EDIT_OFF_HOUR) * 100) + menuGetValue(MENU_EDIT_OFF_MINUTE);
		int timeOn = (menuGetValue(MENU_EDIT_ON_HOUR) * 100) + menuGetValue(MENU_EDIT_ON_MINUTE);
		int currentTime = (hour() * 100) + minute();

		if(timeOff > timeOn) {  // Turned off during night
			if(currentTime > timeOff || currentTime < timeOn) {
				return true;
			}
		}
		else {  // Turned off during day
			if(currentTime > timeOff && currentTime < timeOn) {
				return true;
			}
		}
	}
	return false;
}
