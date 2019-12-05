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
#include <SPI.h>
#include "display.h"
#include "pins.h"

#define UpperDotsMask 0x80000000
#define LowerDotsMask 0x40000000
#define TubeON 0xFFFF
#define TubeOFF 0x3C00

// Array used to calculate the bit pattern for each tube
unsigned int SymbolArray[10]={1, 2, 4, 8, 16, 32, 64, 128, 256, 512};

// Indicates if all the indicators should be powered on (subject to their own values)
volatile boolean powerOn = true;

// Current displayed value in the tubes
String stringToDisplay = "000000";

// Digits to be flashed
unsigned int blinkMask = 0;

// Flags that control if the upper/lower dots are turned on
boolean upperDots = false, lowerDots = false;

void displaySetup()
{
	pinMode(PIN_DISPLAY_ENABLE, OUTPUT);

	SPI.begin();
	SPI.setDataMode (SPI_MODE2); // Mode 3 SPI
	SPI.setClockDivider(SPI_CLOCK_DIV8); // SCK = 16MHz/128= 125kHz

	//timer4 setup for calling doIndication function
	TCCR4A = 0;             //control registers reset (WGM21, WGM20)
	TCCR4B = 0;             //control registers reset.
	TCCR4B = (1 << CS12)|(1 << CS10)|(1 << WGM12); //prescaler 1024 and CTC mode
	//OCR5A = 31; //2 mS
	TCNT4=0; //reset counter to 0
	OCR4A = 46; //3mS
	//OCR4A = 92; //6mS
	TIMSK4 = (1 << OCIE1A);//TIMER4_COMPA_vect interrupt enable
	interrupts();

	displayPowerOn();

}

void displayPowerOn()
{
	powerOn = true;
}

void displayPowerOff()
{
	powerOn = false;
}

boolean displayIsPoweredOn()
{
	return powerOn;
}

void displaySetUpperDots(boolean status)
{
	upperDots = status;
}

void displaySetLowerDots(boolean status)
{
	lowerDots = status;
}

void displaySetBlinkMask(byte newBlinkMask)
{
	blinkMask = newBlinkMask;
}

void displaySetValue(String value)
{
	stringToDisplay = value;
}

word doEditBlink(unsigned int pos)
{
	unsigned int lowBit=blinkMask>>pos;
	lowBit=lowBit&B00000001;

	static unsigned long lastTimeEditBlink=millis();
	static bool blinkState=false;
	word mask=TubeON;
	if ((millis()-lastTimeEditBlink)>300)
	{
		lastTimeEditBlink=millis();
		blinkState=!blinkState;
	}

	if (blinkState && (lowBit == 1)) mask=TubeOFF;//mask=B11111111;
	return mask;
}

word moveMask()
{
	static int callCounter=0;
	static int tubeCounter=0;
	word onOffTubeMask;
	if (callCounter == tubeCounter) onOffTubeMask=TubeON;
	else onOffTubeMask=TubeOFF;
	callCounter++;
	if (callCounter == 3	)
	{
		callCounter=0;
		tubeCounter++;
		if (tubeCounter == 3) tubeCounter = 0;
	}
	return onOffTubeMask;
}

void displayUpdate()
{
	unsigned long Var32;

	// Turning off the OE pin for the tube drivers first, so nothing else is done if display is turned off
	bitClear(PORTB, PB4);

	if(!powerOn) {
		return;
	}

	unsigned long digits=stringToDisplay.toInt();
	Var32 = 0;

	Var32|=(unsigned long)(SymbolArray[digits%10]&doEditBlink(5)&moveMask())<<20; // s2
	digits/=10;

	Var32|=(unsigned long)(SymbolArray[digits%10]&doEditBlink(4)&moveMask())<<10; //s1
	digits/=10;

	Var32|=(unsigned long) (SymbolArray[digits%10]&doEditBlink(3)&moveMask()); //m2
	digits/=10;

	if (lowerDots) {
		Var32|=LowerDotsMask;
	}
	else {
		Var32&=~LowerDotsMask;
	}
	if (upperDots) {
		Var32|=UpperDotsMask;
	}
	else {
		Var32&=~UpperDotsMask;
	}

	SPI.transfer(Var32>>24);
	SPI.transfer(Var32>>16);
	SPI.transfer(Var32>>8);
	SPI.transfer(Var32);

	Var32=0;

	Var32|=(unsigned long)(SymbolArray[digits%10]&doEditBlink(2)&moveMask())<<20; // m1
	digits/=10;

	Var32|= (unsigned long)(SymbolArray[digits%10]&doEditBlink(1)&moveMask())<<10; //h2
	digits/=10;

	Var32|= (unsigned long)SymbolArray[digits%10]&doEditBlink(0)&moveMask(); //h1
	digits/=10;

	if (lowerDots) {
		Var32|=LowerDotsMask;
	}
	else  {
		Var32&=~LowerDotsMask;
	}

	if (upperDots) {
		Var32|=UpperDotsMask;
	}
	else {
		Var32&=~UpperDotsMask;
	}

	SPI.transfer(Var32>>24);
	SPI.transfer(Var32>>16);
	SPI.transfer(Var32>>8);
	SPI.transfer(Var32);

	bitSet(PORTB, PB4);

}

ISR(TIMER4_COMPA_vect)
{
	// We must enable interrupts as early as possible. Otherwise, the beeper will suffer
	interrupts();
	displayUpdate();
}