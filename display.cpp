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
 * The display hardware is composed of 6 IN-18 tubes and 4 neon bulbs (grouped in pairs)
 * connected to two HV5122 shift registers daisy chained together.
 *
 * The connection to the Arduino board uses a pseudo-SPI interface where only signals
 * SCK and MOSI are used. These are connected to D13 and D11, respectively. Additionally,
 * there's an enable signal in the shift registers (OE), which is connected to the D10
 * pin. This pin is used to turn off all the display at once, regardless of the individual
 * bits.
 *
 * This table shows where each digit is connected in the shift register:
 *
 * /=======+=======+=======+=========+=========+=========+=========\
 * | Tube  | Hours | Hours | Minutes | Minutes | Seconds | Seconds |
 * | Digit |  Hi   |  Lo   |   Hi    |   Lo    |   Hi    |   Lo    |
 * +=======+=======+=======+=========+=========+=========+=========+
 * |   0   |   0   |  10   |   20    |   32    |   42    |   52    |
 * |   1   |   1   |  11   |   21    |   33    |   43    |   53    |
 * |   2   |   2   |  12   |   22    |   34    |   44    |   54    |
 * |   3   |   3   |  13   |   23    |   35    |   45    |   55    |
 * |   4   |   4   |  14   |   24    |   36    |   46    |   56    |
 * |   5   |   5   |  15   |   25    |   37    |   47    |   57    |
 * |   6   |   6   |  16   |   26    |   38    |   48    |   58    |
 * |   7   |   7   |  17   |   27    |   39    |   49    |   59    |
 * |   8   |   8   |  18   |   28    |   40    |   50    |   60    |
 * |   9   |   9   |  19   |   29    |   41    |   51    |   61    |
 * \=======+=======+=======+=========+=========+=========+=========/
 *
 * Additionally, the dots for the columns are connected as following:
 * Bit 30: Lower, right dot
 * Bit 31: Upper, right dot
 * Bit 62: Lower, left dot
 * Bit 63: Upper, left dot
 * (this means the upper dots are connected to the highest bit of each
 * 32-bit shift register and the lower dots to the next highest bit)
 *
 * Timer Interrupt 4 is used to automatically refresh the display, so
 * an ISR is used. The interrupt vector is at the bottom of this file.
 *
 */

#include <Arduino.h>
#include <SPI.h>
#include "display.h"
#include "pins.h"

// The bitmask for the upper dot in a 32-bit shift register.
#define UpperDotsMask 0x80000000
// The bitmask for the lower dot in a 32-bit shift register.
#define LowerDotsMask 0x40000000

// Bitmask for turning on all digits in half the shift register
#define TubeON 0xFFFF
// Bitmask for turning off all digits in half the shift register
#define TubeOFF 0x0000

// Array used to calculate the bit pattern for each tube
unsigned int SymbolArray[10]={1, 2, 4, 8, 16, 32, 64, 128, 256, 512};

// Indicates if all the indicators should be powered on (subject to their own values)
volatile bool powerOn = true;

// Current displayed value in the tubes
String stringToDisplay = "000000";

// Digits to be flashed
unsigned int blinkMask = 0;

// Flags that control if the upper/lower dots are turned on
bool upperDots = false, lowerDots = false;

/**
 * This function configures the display. This means setting up the enable pin,
 * configuring SPI and setting up the Timer 4 interrupt.
 */
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

/**
 * Sets the display to be powered on. The actual displayed value will
 * depend on the digit value and the dots statuses.
 */
void displayPowerOn()
{
	powerOn = true;
}

/**
 * Sets the display to be powered off. Any value will be ignored and
 * nothing will be displayed.
 */
void displayPowerOff()
{
	powerOn = false;
}

/**
 * Checks if the display is powered on.
 * @return true if the display is on, false if it isn't.
 */
bool displayIsPoweredOn()
{
	return powerOn;
}

/**
 * Sets if the upper dots (both of them) should be turned on.
 * @param status	True if they should be turned on. False if they shouldn't.
 */
void displaySetUpperDots(bool status)
{
	upperDots = status;
}

/**
 * Sets if the lower dots (both of them) should be turned on.
 * @param status	True if they should be turned on. False if they shouldn't.
 */
void displaySetLowerDots(bool status)
{
	lowerDots = status;
}

/**
 * Sets the blink mask. The blink mask means that for each "1" the corresponding
 * tube will blink. A "0" means that the tube will *not* blink. The bits are
 * assigned to the tubes as follows:
 * - 0: Hours High
 * - 1: Hours Low
 * - 2: Minutes High
 * - 3: Minutes Low
 * - 4: Seconds High
 * - 5: Seconds Low
 * @param newBlinkMask	The new blink mask
 */
void displaySetBlinkMask(byte newBlinkMask)
{
	blinkMask = newBlinkMask;
}

/**
 * Sets the value to be displayed. It must consist of exactly 6 digits in
 * the same order as they are displayed.
 * @param value	The digits to display.
 */
void displaySetValue(String value)
{
	stringToDisplay = value;
}

/**
 * Returns the appropriate mask to make a certain digit blink.
 * @param pos	The digit to blink, as specified in the blink mask's bits.
 * @return	Returns TubeON if the tube should be turned on, or TubeOFF
 * 		if it shouldn't.
 */
static word doEditBlink(unsigned int pos)
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

#define COUNT_CYCLE	6	/* Set to 3 if you want more brightness,
 				 * but it will shorten the tube's life
 				 * a lot. Setting to 6 is the safe choice
 				 * without any hardware modification.
 				 */

/**
 * Turns on or off the tubes every few calls so to restrict the current going
 * through the tubes. This allows controlling the brightness of the tubes.
 * However, note that if they are turned on too frequently will cause them to
 * wear faster, while turning them on too infrequently will cause either a
 * failure to light up or show blue dots. It is suggested that this function
 * is not modified unless you know what you're doing.
 * @return	Returns TubeON if the tube should be turned on, or TubeOFF
 * 		if it shouldn't.
 */
static word moveMask()
{
	static int callCounter=0;
	static int tubeCounter=0;
	word onOffTubeMask;
	if (callCounter == tubeCounter) onOffTubeMask=TubeON;
	else onOffTubeMask=TubeOFF;
	callCounter++;
	if (callCounter == COUNT_CYCLE)
	{
		callCounter=0;
		tubeCounter++;
		if (tubeCounter == COUNT_CYCLE) tubeCounter = 0;
	}
	return onOffTubeMask;
}

/**
 * This function updates the display. This consists of the following procedure:
 * - Turn off the tubes by setting low the OE line of the shift registers.
 * - Prepare the bits to send to the shift register, taking into account the
 *   blinking and the move mask.
 * - Send the bits using SPI.
 * - Turn on the tubes by setting high the OE line of the shift registers.
 *
 * Note: if the display is turned off, this function will only set the OE line
 * to low so nothing is shown regardless of any value set.
 */
static void displayUpdate()
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

/**
 * ISR for Timer 4
 * This vector is used to enable interrupts and call the displayUpdate() function. It is
 * critical that interrupts are re-enabled as soon as possible to avoid affecting other
 * routines that are time-critical, such as the beeper.
 */
ISR(TIMER4_COMPA_vect)
{
	// We must enable interrupts as early as possible. Otherwise, the beeper will suffer
	interrupts();
	displayUpdate();
}