#include <OneWire.h>
#include "ds18b20.h"

#define pinTemp 7

OneWire ds(pinTemp);

boolean isPresent = false;

byte addr[8];

void ds18b20Setup()
{
	if(ds.search(addr)) {
		isPresent = true;
	}
}

boolean ds18b20IsPresent()
{
	return isPresent;
}

/*float getTemperature (boolean bTempFormat)
{
  byte TempRawData[2];
  ds.reset();
  ds.write(0xCC); //skip ROM command
  ds.write(0x44); //send make convert to all devices
  ds.reset();
  ds.write(0xCC); //skip ROM command
  ds.write(0xBE); //send request to all devices

  float celsius = (float)raw / 16.0;
  float fDegrees;
  if (!bTempFormat) fDegrees = celsius * 10;
  else fDegrees = (celsius * 1.8 + 32.0) * 10;
  //Serial.println(fDegrees);
  return fDegrees;
}*/

float ds18b20ReadTemperature()
{
	byte TempRawData[2];
	ds.reset();
	ds.write(0xCC); //skip ROM command
	ds.write(0x44); //send make convert to all devices
	ds.reset();
	ds.write(0xCC); //skip ROM command
	ds.write(0xBE); //send request to all devices
	TempRawData[0] = ds.read();
	TempRawData[1] = ds.read();
	int16_t raw = (TempRawData[1] << 8) | TempRawData[0];
	if (raw == -1) raw = 0;
	return (float)raw / 16.0;
}