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
#ifndef IN18CLOCK_MENU_H
#define IN18CLOCK_MENU_H

#define NO_PARENT	-1
#define	NO_CHILD	-1
#define NO_LOAD		-1

typedef struct menu_struct
{
	int 	parent;
	int 	firstChild;
	int 	lastChild;
	int 	value;
	int 	minValue;
	int 	maxValue;
	int 	eepromOffset;
	int 	blinkPattern;
	void	(*displayHandler)();
	boolean (*onEditHandler)();
	void	(*onIncrementHandler)();
	void	(*onDecrementHandler)();
	void	(*onSaveHandler)();
	void	(*onShowHandler)();
};

void menuSetup(menu_struct *new_menu, int numElements);
int menuGetCurrentPosition();
void menuUpdate();
void menuSetValue(int index, int value);
int menuGetValue(int index);
void menuSetBlinkPattern(int index, int blinkPattern);
int menuSave(int index);

#endif //IN18CLOCK_MENU_H
