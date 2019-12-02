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
#include "gps.h"

#define GPS_BUFFER_LENGTH 83
char GPS_Package[GPS_BUFFER_LENGTH];
byte GPS_position=0;

struct GPS_DATE_TIME
{
	byte GPS_hours;
	byte GPS_minutes;
	byte GPS_seconds;
	byte GPS_day;
	byte GPS_month;
	int GPS_year;
	bool GPS_Valid_Data=false;
	unsigned long GPS_Data_Parsed_time;
};

GPS_DATE_TIME GPS_Date_Time;

void gpsSetup()
{
	Serial1.begin(9600);
}

uint8_t gpsVerifyChecksum()
{
	uint8_t  CheckSum = 0, MessageCheckSum = 0;   // check sum
	uint16_t i = 1;                // 1 sybol left from '$'

	while (GPS_Package[i]!='*')
	{
		CheckSum^=GPS_Package[i];
		if (++i == GPS_BUFFER_LENGTH) {Serial.println(F("End of the line")); return 0;} // end of line not found
	}

	if (GPS_Package[++i]>0x40) MessageCheckSum=(GPS_Package[i]-0x37)<<4;  // ASCII codes to DEC convertation
	else                  MessageCheckSum=(GPS_Package[i]-0x30)<<4;
	if (GPS_Package[++i]>0x40) MessageCheckSum+=(GPS_Package[i]-0x37);
	else                  MessageCheckSum+=(GPS_Package[i]-0x30);

	if (MessageCheckSum != CheckSum) {Serial.println("wrong checksum"); return 0;} // wrong checksum
	//Serial.println("Checksum is ok");
	return 1; // all ok!
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
				//gpsSetTime();
			}
		}
	}
}