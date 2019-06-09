#ifndef p10_singlecolor
#define p10_singlecolor

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <DMD2.h>
#include <DMD_Nepali.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <animation.h>
#include <EEPROM.h>



class singlecolor
{
  public:
    singlecolor(DMDFrame &dmdFrame, SPIDMD &dmd, ESP8266WebServer &serve, animation &motion,  int width, int height);
    void begin();
    void displayBold(int display_coordinate, int boarder_coordinate_header, int boarder_coordinate_footer);
    void displayNepali(int display_coordinate, int boarder_coordinate_header, int boarder_coordinate_footer);
    void displayNormal(int display_coordinate, int boarder_coordinate_header, int boarder_coordinate_footer);

  private:
    String Message = "";
    DMDFrame &dmd;
    SPIDMD &dmdbright;
    ESP8266WebServer &serve;
    animation &motion;
    int width;
    int height;

    void eeprom_read();
};

#endif
