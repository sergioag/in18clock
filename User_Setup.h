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
#ifndef _IN18CLOCK_USER_SETUP_H_
#define _IN18CLOCK_USER_SETUP_H_

void doTest();

#define MENU_TIME		0
#define MENU_DATE		1
#define MENU_TEMPERATURE	2
#define MENU_ALARM		3
#define MENU_LED_CONFIG		4
#define MENU_ONOFF_CONFIG	5
#define MENU_EDIT_TIME_FORMAT	6
#define MENU_EDIT_UTC_SIGN	7
#define MENU_EDIT_UTC_HOURS	8
#define MENU_EDIT_UTC_MINUTES	9
#define MENU_EDIT_HOURS		10
#define MENU_EDIT_MINUTES	11
#define MENU_EDIT_SECONDS	12
#define MENU_EDIT_DATE_FORMAT	13
#define MENU_EDIT_DAY		14
#define MENU_EDIT_MONTH		15
#define MENU_EDIT_YEAR		16
#define MENU_EDIT_TEMP_UNIT	17
#define MENU_EDIT_AL_HOURS	18
#define MENU_EDIT_AL_MINUTES	19
#define MENU_EDIT_AL_SECONDS	20
#define MENU_EDIT_AL_ENABLE	21
#define MENU_EDIT_AL_SONG	22
#define MENU_EDIT_LED_MODE	23
#define MENU_EDIT_LED_RED	24
#define MENU_EDIT_LED_GREEN	25
#define MENU_EDIT_LED_BLUE	26
#define MENU_EDIT_OFF_HOUR	27
#define MENU_EDIT_OFF_MINUTE	28
#define MENU_EDIT_ON_HOUR	29
#define MENU_EDIT_ON_MINUTE	30
#define MENU_EDIT_ONOFF_ENABLE	31

#endif //_IN18CLOCK_USER_SETUP_H_
