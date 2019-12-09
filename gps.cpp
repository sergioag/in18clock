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
#include "debug.h"
#include "gps.h"
#include "rtc.h"

#define GPS_BUFFER_LENGTH 83
static char GPS_Package[GPS_BUFFER_LENGTH];
static byte GPS_position=0;

struct GPS_DATE_TIME
{
	byte GPS_hours;
	byte GPS_minutes;
	byte GPS_seconds;
	byte GPS_day;
	byte GPS_month;
	int GPS_year;
	bool GPS_Valid_Data=false;
};

GPS_DATE_TIME GPS_Date_Time;

void gpsSetup()
{
	Serial1.begin(9600);
}

bool gpsVerifyChecksum()
{
	uint8_t  CheckSum = 0, MessageCheckSum = 0;   // checksum
	uint16_t i = 1;                // 1 symbol left from '$'

	while (GPS_Package[i]!='*')
	{
		CheckSum^=GPS_Package[i];
		if (++i == GPS_BUFFER_LENGTH) {
			debugOutput("End of line");
			return false;
		} // end of line not found
	}

	if (GPS_Package[++i]>0x40)
		MessageCheckSum=(GPS_Package[i]-0x37)<<4;  // ASCII codes to DEC convertation
	else
		MessageCheckSum=(GPS_Package[i]-0x30)<<4;

	if (GPS_Package[++i]>0x40)
		MessageCheckSum+=(GPS_Package[i]-0x37);
	else
		MessageCheckSum+=(GPS_Package[i]-0x30);

	if (MessageCheckSum != CheckSum) {
		debugOutput("wrong checksum");
		return false;
	}
	return true;
}

static bool inRange( int no, int low, int high )
{
	return !(no < low || no > high);
}

void gpsTimeUpdate()
{
	if(GPS_Date_Time.GPS_Valid_Data) {
		GPS_Date_Time.GPS_Valid_Data = false;

		rtc_info *rtcInfo = rtcGetTime();
		rtcInfo->years = GPS_Date_Time.GPS_year;
		rtcInfo->months = GPS_Date_Time.GPS_month;
		rtcInfo->days = GPS_Date_Time.GPS_day;
		rtcInfo->hours = GPS_Date_Time.GPS_hours;
		rtcInfo->minutes = GPS_Date_Time.GPS_minutes;
		rtcInfo->seconds = GPS_Date_Time.GPS_seconds;

		rtcSetTime(rtcInfo);
	}
}

void gpsParseTime()
{
	if (!((GPS_Package[0]   == '$')
	      &&(GPS_Package[3] == 'R')
	      &&(GPS_Package[4] == 'M')
	      &&(GPS_Package[5] == 'C'))) {return;}

	int hh=(GPS_Package[7]-48)*10+GPS_Package[8]-48;
	int mm=(GPS_Package[9]-48)*10+GPS_Package[10]-48;
	int ss=(GPS_Package[11]-48)*10+GPS_Package[12]-48;

	byte GPSDatePos=0;
	int CommasCounter=0;
	for (int i = 12; i < GPS_BUFFER_LENGTH ; i++)
	{
		if (GPS_Package[i] == ',')
		{
			CommasCounter++;
			if (CommasCounter==8)
			{
				GPSDatePos=i+1;
				break;
			}
		}
	}
	int dd=(GPS_Package[GPSDatePos]-48)*10+GPS_Package[GPSDatePos+1]-48;
	int MM=(GPS_Package[GPSDatePos+2]-48)*10+GPS_Package[GPSDatePos+3]-48;
	int yyyy=2000+(GPS_Package[GPSDatePos+4]-48)*10+GPS_Package[GPSDatePos+5]-48;
	//if ((hh<0) || (mm<0) || (ss<0) || (dd<0) || (MM<0) || (yyyy<0)) return false;
	if ( !inRange( yyyy, 2018, 2038 ) ||
	     !inRange( MM, 1, 12 ) ||
	     !inRange( dd, 1, 31 ) ||
	     !inRange( hh, 0, 23 ) ||
	     !inRange( mm, 0, 59 ) ||
	     !inRange( ss, 0, 59 ) ) return;
	else
	{
		GPS_Date_Time.GPS_hours=hh;
		GPS_Date_Time.GPS_minutes=mm;
		GPS_Date_Time.GPS_seconds=ss;
		GPS_Date_Time.GPS_day=dd;
		GPS_Date_Time.GPS_month=MM;
		GPS_Date_Time.GPS_year=yyyy;
		GPS_Date_Time.GPS_Valid_Data = true;
		return;
	}

}

void gpsUpdate()
{
	if(Serial1.available()) {
		byte readByte = Serial1.read();
		GPS_Package[GPS_position++] = readByte;
		if(GPS_position == GPS_BUFFER_LENGTH) {
			GPS_position = 0;
		}
		else if(readByte == 0x0a) {
			GPS_Package[GPS_position] = 0;
			GPS_position = 0;
			if(gpsVerifyChecksum()) {
				gpsParseTime();
			}
		}
	}
}