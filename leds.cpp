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
#include <Adafruit_NeoPixel.h>
#include "User_Setup.h"
#include "display.h"
#include "leds.h"
#include "menu.h"
#include "pins.h"
#include "utils.h"

#define NUMPIXELS      8
#define LEDsSpeed      10
const int LEDsDelay=40;
unsigned long prevTime4FireWorks = 0; //time of last RGB changed
int RedLight = 255;
int GreenLight = 0;
int BlueLight = 0;
int rotator = 0; //index in array with RGB "rules" (increse by one on each 255 cycles)
int cycle = 0; //cycles counter

int fireworks[] = {0, 0, 1, //1
		   -1, 0, 0, //2
		   0, 1, 0, //3
		   0, 0, -1, //4
		   1, 0, 0, //5
		   0, -1, 0
}; //array with RGB rules (0 - do nothing, -1 - decrese, +1 - increse

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN_LEDS, NEO_GRB + NEO_KHZ800);
int currentRed = 0, currentGreen = 0, currentBlue = 0;

void ledsSetup()
{
	pixels.begin(); // This initializes the NeoPixel library.
	pixels.setBrightness(50);
}

void ledsOff()
{
	ledsSet(0,0,0);
}

void ledsTest()
{
	ledsSet(255,0,0);
	delay(1000);
	ledsSet(0,255,0);
	delay(1000);
	ledsSet(0, 0, 255);
	delay(1000);
	ledsOff();
}

void rotateFireWorks()
{
	RedLight = RedLight + LEDsSpeed * fireworks[rotator * 3];
	GreenLight = GreenLight + LEDsSpeed * fireworks[rotator * 3 + 1];
	BlueLight = BlueLight + LEDsSpeed * fireworks[rotator * 3 + 2];

	ledsSet(RedLight, GreenLight, BlueLight);

	cycle = cycle + 1;
	if (cycle == 255/LEDsSpeed)
	{
		rotator = rotator + 1;
		cycle = 0;
	}
	if (rotator > 5) rotator = 0;
}

void ledsSet(int red, int green, int blue)
{
	// Since NeoPixel library disables interrupts, we want to keep the calls to it to a minimum. Only call it
	// if we actually need to change colors.
	if(red != currentRed || green != currentGreen || blue != currentBlue) {
		for(int i=0;i<NUMPIXELS;i++)
		{
			pixels.setPixelColor(i, pixels.Color(red, green, blue));
		}
		pixels.show();

		currentRed = red;
		currentGreen = green;
		currentBlue = blue;
	}
}

void ledsUpdate()
{
	if(!displayIsPoweredOn()) {
		ledsOff();
	}
	else {
		if ((millis() - prevTime4FireWorks) > LEDsDelay)
		{
			switch(menuGetValue(MENU_EDIT_LED_MODE)) {
				case LEDS_OFF:
					ledsOff();
					break;
				case LEDS_FIREWORKS:
					rotateFireWorks(); //change color (by 1 step)
					break;
				case LEDS_FIXED:
					ledsSet(menuGetValue(MENU_EDIT_LED_RED)*4, menuGetValue(MENU_EDIT_LED_GREEN)*4, menuGetValue(MENU_EDIT_LED_BLUE)*4);
					break;
			}
			prevTime4FireWorks = millis();
		}
	}
}

void ledsDisplay()
{
	if(menuGetCurrentPosition() == MENU_EDIT_LED_MODE) {
		displaySetValue("00" + PreZero(menuGetValue(MENU_EDIT_LED_MODE)) + "00");
	}
	else {
		displaySetValue(PreZero(menuGetValue(MENU_EDIT_LED_RED))
				+ PreZero(menuGetValue(MENU_EDIT_LED_GREEN))
				+ PreZero(menuGetValue(MENU_EDIT_LED_BLUE)));
	}
	displaySetUpperDots(false);
	displaySetLowerDots(false);
}

void ledsOnSave()
{
	menuSave(MENU_EDIT_LED_MODE);
	menuSave(MENU_EDIT_LED_RED);
	menuSave(MENU_EDIT_LED_GREEN);
	menuSave(MENU_EDIT_LED_BLUE);
}
