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
#include <ClickButton.h>
#include <EEPROM.h>
#include "alarm.h"
#include "debug.h"
#include "display.h"
#include "ir.h"
#include "menu.h"
#include "pins.h"
#include "tone.h"

#define CLICK_LONG	-1
#define CLICK_SHORT	1

#define INPUT_TIMEOUT	10000

ClickButton modeButton(PIN_BUTTON_MODE, LOW, CLICKBTN_PULLUP);
ClickButton upButton(PIN_BUTTON_UP, LOW, CLICKBTN_PULLUP);
ClickButton downButton(PIN_BUTTON_DOWN, LOW, CLICKBTN_PULLUP);

int menuPosition = 0;
int numMenuElements = 0;
unsigned long timeLastInput = 0;


static menu_struct *menu;


void menuSetPosition(int newId)
{
	int newPosition = menuGetIndexById(newId);

	if(menu[menuPosition].onShowHandler) {
		menu[menuPosition].onShowHandler(false);
	}
	menuPosition = newPosition;
	displaySetBlinkMask(menu[menuPosition].blinkPattern);
	if(menu[menuPosition].onShowHandler) {
		menu[menuPosition].onShowHandler(true);
	}
}

void menuSetup(menu_struct *new_menu, int numElements)
{

	pinMode(PIN_BUTTON_MODE,  INPUT_PULLUP);
	pinMode(PIN_BUTTON_UP,  INPUT_PULLUP);
	pinMode(PIN_BUTTON_DOWN,  INPUT_PULLUP);

	modeButton.debounceTime   = 20;   // Debounce timer in ms
	modeButton.multiclickTime = 30;  // Time limit for multi clicks
	modeButton.longClickTime  = 2000; // time until "held-down clicks" register

	upButton.debounceTime   = 20;   // Debounce timer in ms
	upButton.multiclickTime = 30;  // Time limit for multi clicks
	upButton.longClickTime  = 2000; // time until "held-down clicks" register

	downButton.debounceTime   = 20;   // Debounce timer in ms
	downButton.multiclickTime = 30;  // Time limit for multi clicks
	downButton.longClickTime  = 2000; // time until "held-down clicks" register

	irSetup();

	menu = new_menu;
	numMenuElements = numElements;

	for(int i = 0; i < numElements; i++) {
		if(menu[i].eepromOffset != NO_LOAD) {
			uint8_t value = EEPROM.read(menu[i].eepromOffset);
			if(value != 255) {
				if(value < menu[i].minValue) value = menu[i].minValue;
				if(value > menu[i].maxValue) value = menu[i].maxValue;
				menu[i].value = value;
			}
		}
	}

	menuSetPosition(0);
	timeLastInput = millis();
}

void menuTryEdit()
{
	if(menu[menuPosition].firstChild != NO_CHILD && (!menu[menuPosition].onEditHandler || menu[menuPosition].onEditHandler())) {
		toneKey();
		menuSetPosition(menu[menuPosition].firstChild);
	}
}

void menuBack()
{
	if(menu[menuPosition].parent != NO_PARENT) {
		menuSetPosition(menu[menuPosition].parent);
		if(menu[menuPosition].onSaveHandler) {
			menu[menuPosition].onSaveHandler();
		}
	}
	else {
		menuSetPosition(0);
	}
}

void menuNext()
{
	int newMenuPosition = menuPosition + 1;
	toneKey();

	if(menu[menuPosition].parent == NO_PARENT) {
		/*
		 * When advancing from parent, we cannot use parent->lastChild since there is no parent. Hence,
		 * we have two cases:
		 * - If we've gone through the end of the menu, roll back to the start.
		 * - If the next menu is not a parent menu, roll back to the start.
		 */
		if(newMenuPosition == numMenuElements || menu[newMenuPosition].parent != NO_PARENT) {
			menuSetPosition(0);
		}
		else {
			menuSetPosition(menu[newMenuPosition].id);
		}
	}
	else {
		/*
		 * If we reach the lastChild or reach the end of menu (shouldn't happen), then we return to parent.
		 */
		if(newMenuPosition == numMenuElements || newMenuPosition > menuGetIndexById(menu[menuGetIndexById(menu[menuPosition].parent)].lastChild)) {
			menuBack();
		}
		else {
			menuSetPosition(menu[newMenuPosition].id);
		}
	}
}

void menuIncrement()
{
	if(menu[menuPosition].onIncrementHandler) {
		menu[menuPosition].onIncrementHandler();
	}
	else {
		menu[menuPosition].value++;
		if(menu[menuPosition].value > menu[menuPosition].maxValue) {
			menu[menuPosition].value = menu[menuPosition].minValue;
		}
	}
	toneKey();
}

void menuDecrement()
{
	if(menu[menuPosition].onDecrementHandler) {
		menu[menuPosition].onDecrementHandler();
	}
	else {
		menu[menuPosition].value--;
		if(menu[menuPosition].value < menu[menuPosition].minValue) {
			menu[menuPosition].value = menu[menuPosition].maxValue;
		}
	}
	toneKey();
}

void menuUpdate()
{
	if(menu[menuPosition].displayHandler) {
		menu[menuPosition].displayHandler();
	}

	modeButton.Update();
	upButton.Update();
	downButton.Update();
	irUpdate();

	if(alarmIsSounding()) {
		// When alarm is sounding, any button stops it
		if(menuModeButtonState() != 0 || menuUpButtonState() != 0 || menuDownButtonState() != 0) {
			alarmStop();
		}
	}
	else {
		switch(menuModeButtonState()) {
			case CLICK_LONG:
				timeLastInput = millis();
				menuTryEdit();
				return;
			case CLICK_SHORT:
				timeLastInput = millis();
				menuNext();
				return;

		}
		switch(menuUpButtonState()) {
			case CLICK_LONG:
				// TODO: Do something with long up clicks
				break;
			case CLICK_SHORT:
				timeLastInput = millis();
				menuIncrement();
				return;
		}
		switch(menuDownButtonState()) {
			case CLICK_LONG:
				// TODO: Do something with long down clicks
				break;
			case CLICK_SHORT:
				timeLastInput = millis();
				menuDecrement();
				return;
		}
	}

	if (menuPosition != 0 && millis() - timeLastInput > INPUT_TIMEOUT) {
		timeLastInput = millis();
		menuBack();
	}
}

int menuGetCurrentPosition()
{
	return menu[menuPosition].id;
}

void menuSetValue(int id, int value)
{
	menu[menuGetIndexById(id)].value = value;
}

int menuGetValue(int id)
{
	return menu[menuGetIndexById(id)].value;
}

void menuSetBlinkPattern(int id, int blinkPattern)
{
	menu[menuGetIndexById(id)].blinkPattern = blinkPattern;
}

int menuSave(int id)
{
	int index = menuGetIndexById(id);
	if(menu[index].eepromOffset != NO_LOAD) {
		EEPROM.update(menu[index].eepromOffset, menu[index].value);
	}
}

int menuModeButtonState()
{
	if(modeButton.clicks) {
		return modeButton.clicks;
	}
	else {
		return irModeButton();
	}
}

int menuUpButtonState()
{
	if(upButton.clicks) {
		return upButton.clicks;
	}
	else {
		return irUpButton();
	}
}

int menuDownButtonState()
{
	if(downButton.clicks) {
		return downButton.clicks;
	}
	else {
		return irDownButton();
	}
}

int menuGetIndexById(int id)
{
	for(int i = 0; i < numMenuElements; i++) {
		if(menu[i].id == id) {
			return i;
		}
	}

	debugOutput("Returning root menu because menu was not found with ID ", id);

	return 0;
}