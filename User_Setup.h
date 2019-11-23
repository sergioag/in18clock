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

#define MENU_EDIT_TIME_FORMAT	6
#define MENU_EDIT_HOURS		7
#define MENU_EDIT_MINUTES	8
#define MENU_EDIT_SECONDS	9
#define MENU_EDIT_DATE_FORMAT	10
#define MENU_EDIT_DAY		11
#define MENU_EDIT_MONTH		12
#define MENU_EDIT_YEAR		13
#define MENU_EDIT_TEMP_UNIT	14
#define MENU_EDIT_AL_HOURS	15
#define MENU_EDIT_AL_MINUTES	16
#define MENU_EDIT_AL_SECONDS	17
#define MENU_EDIT_AL_ENABLE	18
#define MENU_EDIT_AL_SONG	19
#define MENU_EDIT_LED_MODE	20
#define MENU_EDIT_LED_RED	21
#define MENU_EDIT_LED_GREEN	22
#define MENU_EDIT_LED_BLUE	23
#define MENU_EDIT_OFF_HOUR	24
#define MENU_EDIT_OFF_MINUTE	25
#define MENU_EDIT_ON_HOUR	26
#define MENU_EDIT_ON_MINUTE	27
#define MENU_EDIT_ONOFF_ENABLE	28

#endif //_IN18CLOCK_USER_SETUP_H_
