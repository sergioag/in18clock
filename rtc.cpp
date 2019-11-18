#include <Time.h>
#include <Wire.h>
#include "rtc.h"

#define DS3231_ADDRESS	0x68
byte zero = 0x00;

rtc_info rtcInfo;

byte decToBcd(byte val) {
	// Convert normal decimal numbers to binary coded decimal
	return ( (val / 10 * 16) + (val % 10) );
}

byte bcdToDec(byte val)  {
	// Convert binary coded decimal to normal decimal numbers
	return ( (val / 16 * 10) + (val % 16) );
}

void rtcSetup()
{
	Wire.begin();
}

rtc_info *rtcGetTime()
{
	Wire.beginTransmission(DS3231_ADDRESS);
	Wire.write(zero);
	Wire.endTransmission();

	Wire.requestFrom(DS3231_ADDRESS, 7);

	rtcInfo.seconds = bcdToDec(Wire.read());
	rtcInfo.minutes = bcdToDec(Wire.read());
	rtcInfo.hours = bcdToDec(Wire.read() & 0b111111); //24 hour time
	rtcInfo.day_of_week = bcdToDec(Wire.read()); //0-6 -> sunday - Saturday
	rtcInfo.days = bcdToDec(Wire.read() & 0b01111111); // exclude century bit
	rtcInfo.months = bcdToDec(Wire.read());
	rtcInfo.years = bcdToDec(Wire.read());

	// Update the boards RTC, since this is the one we use
	setTime(rtcInfo.hours, rtcInfo.minutes, rtcInfo.seconds, rtcInfo.days, rtcInfo.months, rtcInfo.years);
	return &rtcInfo;
}

void rtcSetTime(rtc_info *rtcInfo)
{
	Wire.beginTransmission(DS3231_ADDRESS);
	Wire.write(zero);

	Wire.write(decToBcd(rtcInfo->seconds));
	Wire.write(decToBcd(rtcInfo->minutes));
	Wire.write(decToBcd(rtcInfo->hours));
	Wire.write(decToBcd(rtcInfo->day_of_week));
	Wire.write(decToBcd(rtcInfo->days));
	Wire.write(decToBcd(rtcInfo->months));
	Wire.write(decToBcd(rtcInfo->years));

	Wire.write(zero); //ensure oscillator is running

	Wire.endTransmission();

	// Update the boards RTC, since this is the one we use
	setTime(rtcInfo->hours, rtcInfo->minutes, rtcInfo->seconds, rtcInfo->days, rtcInfo->months, rtcInfo->years);

}
