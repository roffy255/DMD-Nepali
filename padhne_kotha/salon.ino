//---------------animation clipart-------------------//


#include <Wire.h>
#include <SPI.h>
#include <DMD2.h>
#include <fonts/SystemFont5x7.h>

const int width PROGMEM = 1;
const int height PROGMEM = 1;


SoftDMD dmd(width,height);

void setup()
{
   Wire.begin();
   Serial.begin(9600);
   dmd.begin();
   dmd.selectFont(SystemFont5x7);
   dmd.setBrightness(255);
}

void const_boarder()
{
  for(int i = 0; i<= width*32; i++)
  {
   dmd.setPixel(i, 0, GRAPHICS_ON);
   dmd.setPixel(i, 1, GRAPHICS_ON);
   
  }

  for(int i = width*32; i>= 0; i--)
  {
   dmd.setPixel(i, 15, GRAPHICS_ON);
   dmd.setPixel(i, 14, GRAPHICS_ON);
   
  }
}

void cup(int x, int y)
{
    dmd.setPixel(x,y, GRAPHICS_ON);
    dmd.setPixel(x+1,y+1, GRAPHICS_ON);
    dmd.setPixel(x+2,y+2, GRAPHICS_ON);
    
}

void loop()
{
const_boarder();
}