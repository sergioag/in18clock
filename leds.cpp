#include <Adafruit_NeoPixel.h>

#define PIN            6
#define NUMPIXELS      8
#define LEDsSpeed      10
const int LEDsDelay=40;
unsigned long prevTime4FireWorks = 0; //time of last RGB changed
int RedLight = 255;
int GreenLight = 0;
int BlueLight = 0;
int rotator = 0; //index in array with RGB "rules" (increse by one on each 255 cycles)
int cycle = 0; //cycles counter

int fireworks[] = {0, 0, 1, //1
		   -1, 0, 0, //2
		   0, 1, 0, //3
		   0, 0, -1, //4
		   1, 0, 0, //5
		   0, -1, 0
}; //array with RGB rules (0 - do nothing, -1 - decrese, +1 - increse

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void ledsSetup()
{
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.setBrightness(50);
}

void ledsOff()
{
  for(int i=0;i<NUMPIXELS;i++)
  {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0)); 
  }
  pixels.show();
}

void ledsTest()
{
  for(int i=0;i<NUMPIXELS;i++)
  {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0)); 
  }
  pixels.show(); // This sends the updated pixel color to the hardware.
  delay(1000);
  for(int i=0;i<NUMPIXELS;i++)
  {
    pixels.setPixelColor(i, pixels.Color(0, 255, 0)); 
  }
  pixels.show(); // This sends the updated pixel color to the hardware.
  delay(1000);
  for(int i=0;i<NUMPIXELS;i++)
  {
    pixels.setPixelColor(i, pixels.Color(0, 0, 255)); 
  }
  pixels.show(); // This sends the updated pixel color to the hardware.
  delay(1000);
  ledsOff();
}

void rotateFireWorks()
{
  RedLight = RedLight + LEDsSpeed * fireworks[rotator * 3];
  GreenLight = GreenLight + LEDsSpeed * fireworks[rotator * 3 + 1];
  BlueLight = BlueLight + LEDsSpeed * fireworks[rotator * 3 + 2];

  for(int i=0;i<NUMPIXELS;i++)
  {
    pixels.setPixelColor(i, pixels.Color(RedLight, GreenLight, BlueLight)); 
  }

  pixels.show(); // This sends the updated pixel color to the hardware.
  
  cycle = cycle + 1;
  if (cycle == 255/LEDsSpeed)
  {
    rotator = rotator + 1;
    cycle = 0;
  }
  if (rotator > 5) rotator = 0;
}

void ledsUpdate()
{
	if ((millis() - prevTime4FireWorks) > LEDsDelay)
	{
		rotateFireWorks(); //change color (by 1 step)
		prevTime4FireWorks = millis();
	}
}

/*
void setLEDsFromEEPROM()
{
  int R,G,B;
  R=EEPROM.read(LEDsRedValueEEPROMAddress);
  G=EEPROM.read(LEDsGreenValueEEPROMAddress);
  B=EEPROM.read(LEDsBlueValueEEPROMAddress);

  for(int i=0;i<NUMPIXELS;i++)
  {
    pixels.setPixelColor(i, pixels.Color(R, G, B)); 
  }
  pixels.show();

    // ********
  Serial.println(F("Read from EEPROM"));
  Serial.print(F("RED="));
  Serial.println(R);
  Serial.print(F("GREEN="));
  Serial.println(G);
  Serial.print(F("Blue="));
  Serial.println(B);
  // ********
}
*/