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
#include <NeoGPS.h>
#include "gps.h"
#include "menu.h"
#include "rtc.h"
#include "debug.h"

NMEAGPS gps;
gps_fix gpsFix;

void gpsSetup()
{
	Serial1.begin(9600);
	gpsFix.valid.time = false;
}

void gpsTimeUpdate()
{

	if ((gpsFix.valid.time) && (gpsFix.valid.date) && (gpsFix.status>=3))
	{
		rtc_info rtcInfo;

		rtcInfo.hours = gpsFix.dateTime.hours;
		rtcInfo.minutes = gpsFix.dateTime.minutes;
		rtcInfo.seconds = gpsFix.dateTime.seconds;
		rtcInfo.days = gpsFix.dateTime.date;
		rtcInfo.months = gpsFix.dateTime.month;
		rtcInfo.years = gpsFix.dateTime.year;

		rtcUpdateFromUTC(&rtcInfo);

		gpsFix.valid.time = false;
	}
}

void gpsUpdate()
{
	if(gps.available(Serial1)) {
		gpsFix = gps.read();
	}
}