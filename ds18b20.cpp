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
#include <OneWire.h>
#include "ds18b20.h"
#include "pins.h"

OneWire ds(PIN_DS18B20);

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