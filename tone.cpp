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
#include <Tone.h>
#include "pins.h"
#include "tone.h"

#define OCTAVE_OFFSET 0

int notes[] = { 0,
		NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
		NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
		NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
		NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7
};

Tone tone1;
char *currentSong;
static unsigned long lastTimeNotePlaying = 0;
int num;
long duration;
long wholenote;
int bpm = 63;
byte note;
byte default_dur = 4;
byte default_oct = 6;
byte scale;

void toneSetup()
{
	pinMode(PIN_BUZZER, OUTPUT);
	tone1.begin(PIN_BUZZER);
	currentSong = NULL;
}

void toneKey()
{
	if(!toneIsPlaying()) {
		tone1.play(1000, 100);
	}
}

void toneStop()
{
	currentSong = NULL;
}

void tonePlay(char *song)
{
	// Absolutely no error checking in here
	// format: d=N,o=N,b=NNN:
	// find the start (skip name, etc)

	while (*song != ':') song++;   // ignore name
	song++;                     // skip ':'

	// get default duration
	if (*song == 'd')
	{
		song++; song++;              // skip "d="
		num = 0;
		while (isdigit(*song))
		{
			num = (num * 10) + (*song++ - '0');
		}
		if (num > 0) default_dur = num;
		song++;                   // skip comma
	}

	// get default octave
	if (*song == 'o')
	{
		song++; song++;              // skip "o="
		num = *song++ - '0';
		if (num >= 3 && num <= 7) default_oct = num;
		song++;                   // skip comma
	}

	// get BPM
	if (*song == 'b')
	{
		song++; song++;              // skip "b="
		num = 0;
		while (isdigit(*song))
		{
			num = (num * 10) + (*song++ - '0');
		}
		bpm = num;
		song++;                   // skip colon
	}

	// BPM usually expresses the number of quarter notes per minute
	wholenote = (60 * 1000L / bpm) * 4;  // this is the time for whole note (in milliseconds)
	currentSong = song;
}

void toneUpdate()
{
	if(!toneIsPlaying()) {
		return;
	}
	if (millis() - lastTimeNotePlaying > duration)
		lastTimeNotePlaying = millis();
	else return;
	// first, get note duration, if available
	num = 0;
	while (isdigit(*currentSong))
	{
		num = (num * 10) + (*currentSong++ - '0');
	}

	if (num) duration = wholenote / num;
	else duration = wholenote / default_dur;  // we will need to check if we are a dotted note after

	// now get the note
	note = 0;

	switch (*currentSong)
	{
		case 'c':
			note = 1;
			break;
		case 'd':
			note = 3;
			break;
		case 'e':
			note = 5;
			break;
		case 'f':
			note = 6;
			break;
		case 'g':
			note = 8;
			break;
		case 'a':
			note = 10;
			break;
		case 'b':
			note = 12;
			break;
		case 'p':
		default:
			note = 0;
	}
	currentSong++;

	// now, get optional '#' sharp
	if (*currentSong == '#')
	{
		note++;
		currentSong++;
	}

	// now, get optional '.' dotted note
	if (*currentSong == '.')
	{
		duration += duration / 2;
		currentSong++;
	}

	// now, get scale
	if (isdigit(*currentSong))
	{
		scale = *currentSong - '0';
		currentSong++;
	}
	else
	{
		scale = default_oct;
	}

	scale += OCTAVE_OFFSET;

	if (*currentSong == ',')
		currentSong++;       // skip comma for next note (or we may be at the end)

	// now play the note

	if (note)
	{
		tone1.play(notes[(scale - 4) * 12 + note], duration);
		if (millis() - lastTimeNotePlaying > duration)
			lastTimeNotePlaying = millis();
		else return;
		tone1.stop();
	}
	else
	{
		return;
	}
#ifdef DEBUG
	Serial.println(F("Incorrect Song Format!"));
#endif

}

boolean toneIsPlaying()
{
	return !(currentSong == NULL || *currentSong == 0);
}
