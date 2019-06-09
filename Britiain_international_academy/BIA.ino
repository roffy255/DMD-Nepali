#include <animation.h>
#include <EEPROM.h>
#include <DS3231.h>
#include <Wire.h>
#include <SPI.h>
#include <DMD2.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/NepFont5x7.h>
#include <fonts/Arial_Black_16.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

bool setIPFlag = false;
bool configModeFlag = false;
bool ack = false;

uint16_t offset_address = 0;
uint16_t message_count = 0;
uint16_t actual_addr = 0;

//const String startConfigMsg = "sc";
const char *ssidAP = "FRONT";
const char *passwordAP = "BIA123123";
const uint8_t *FONTEng = SystemFont5x7;
const uint8_t *FONTBold = Arial_Black_16;
const uint8_t width = 3;
const uint8_t height = 3;
String Message = "";
const uint8_t bold_array[96] = {
    10, 3, 7, 11, 9, 14, 11, 3, 5, 5,
    6, 9, 3, 5, 3, 4, 8, 6, 8, 8,
    9, 8, 8, 8, 8, 8, 3, 3, 9, 8,
    9, 8, 12, 12, 9, 9, 9, 9, 8, 10,
    10, 3, 9, 12, 8, 12, 10, 10, 9, 10,
    10, 9, 11, 10, 12, 16, 12, 11, 9, 5,
    4, 5, 8, 8, 3, 9, 9, 9, 9, 9,
    6, 9, 9, 3, 4, 10, 3, 13, 9, 9,
    9, 9, 6, 8, 6, 9, 9, 15, 11, 9,
    7, 6, 2, 6, 9, 8};

ESP8266WebServer server(80);

SPIDMD dmd(width, height);
animation motion(dmd, dmd, width, height);

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  WiFi.softAP(ssidAP, passwordAP);
  delay(500);
  server.on("/", handleRoot);
  server.on("/ba0ca773f607ee97c02bb9d83920d293", handleQueryArgs);
  server.onNotFound(handleNotFound);
  server.begin();
  delay(500);
  dmd.begin();
  EEPROM.begin(512);
  dmd.selectFont(FONTBold);
  dmd.setBrightness(255);
}

void handleRoot()
{
  server.send(200, "text/plain", "hello from server");
}

void handleQueryArgs()
{ //Handler
  String key = server.argName(0);
  String value = server.arg(0);
  if (key == "M")
  {
    Message = value;
    write_to_EEPROM(0);
  }
  else if (key == "B")
  {
    EEPROM.write(510, value.toInt());
    EEPROM.commit();
    set_brightness();
  }
  server.send(200, "text/plain", "i heard you");
}

void handleNotFound()
{
  server.send(404, "text/plain", "Error 404! Incorrect Path");
}

void delay_engine(int delay_val)
{
  unsigned long startMs = millis();
  while (millis() - startMs <= delay_val)
  {
    server.handleClient();
    set_brightness();
    delay(1);
  }
}

void set_brightness()
{
  dmd.setBrightness(EEPROM.read(510));
}

void write_to_EEPROM(uint16_t address)
{
  uint16_t value_len = Message.length();
  uint16_t j = 0;
  if (value_len <= 255)
  {
    EEPROM.write(0, value_len);
    EEPROM.commit();
    EEPROM.write(1, 0);
    EEPROM.commit();
  }
  else if (value_len > 255)
  {
    uint8_t second_value_len = value_len - 255;
    EEPROM.write(0, 255);
    EEPROM.commit();
    EEPROM.write(1, second_value_len);
    EEPROM.commit();
  }
  for (uint16_t i = address; i < (address + value_len); i++)
  {
    char temp_char = Message[j];
    EEPROM.write(i + 2, temp_char);
    EEPROM.commit();
    j++;
  }
  Serial.println("EEPROM writing");
}

void eeprom_read()
{
  Message = "";
  int x = EEPROM.read(0) + EEPROM.read(1);
  for (int i = 0; i < (x); i++)
  {
    char bufferr = EEPROM.read(i + 2);
    Message = String(Message + bufferr);
  }
  Serial.println(Message);
}

void displayBold()
{
  dmd.selectFont(Arial_Black_16);
  eeprom_read();
  uint16_t string_length = Message.length();
  char buffer_char = Message[0];
  int char_ascii_num = int(buffer_char);
  int over_scroll_length = bold_array[char_ascii_num - 32];
  for (int i = width * 32; i > -(over_scroll_length + 1); i--)
  {
    dmd.drawString(i, 16, Message);
    delay_engine(30);
  }
  Message = Message.substring(1, string_length);
  int loop_length = string_length;
  for (int i = 0; i < loop_length; i++)
  {
    string_length = Message.length();
    buffer_char = Message[0];
    char_ascii_num = int(buffer_char);
    over_scroll_length = bold_array[char_ascii_num - 32];

    for (int j = 0; j > -(over_scroll_length + 1); j--)
    {
      dmd.drawString(j, 16, Message);
      delay_engine(30);
    }
    Message = Message.substring(1, string_length);
  }
}

void startAnimation()
{
  australia();
  uk();
  canada();
  newZealand();
  usa();
}

void australia()
{

  delay_engine(1000);
  for (int i = width * 32; i >= 1; i--)
  {
    dmd.drawString(i, 16, "AUSTRALIA");
    delay_engine(20);
  }
  delay_engine(9000);
  for (int i = 16; i > -32; i--)
  {
    dmd.clearScreen();
    if (i > -10)
    {
      dmd.drawString(1, i, "AUSTRALIA");
    }
    motion.operaHouse1(0, i - 16 + 48);
    delay_engine(30);
  }
  delay_engine(5000);
  motion.cleaningCurtain();
  takeOff();
}

void canada()
{

  delay_engine(1000);
  for (int i = width * 32; i >= 14; i--)
  {
    dmd.drawString(i, 16, "CANADA");
    delay_engine(20);
  }
  delay_engine(9000);
  for (int i = 16; i > -32; i--)
  {
    dmd.clearScreen();
    if (i > -10)
    {
      dmd.drawString(14, i, "CANADA");
    }
    motion.canada(0, i - 16 + 48);
    delay_engine(30);
  }
  delay_engine(5000);
  motion.cleaningCurtain();
  takeOff();
}

void uk()
{

  delay_engine(1000);
  for (int i = width * 32; i >= 35; i--)
  {
    dmd.drawString(i, 16, "UK");
    delay_engine(20);
  }
  delay_engine(9000);
  for (int i = 16; i > -32; i--)
  {
    dmd.clearScreen();
    if (i > -10)
    {
      dmd.drawString(35, i, "UK");
    }

    motion.londonBridge(0, i - 16 + 48);
    delay_engine(30);
  }
  delay_engine(5000);
  motion.cleaningCurtain();
  takeOff();
}

void newZealand()
{

  delay_engine(1000);
  // for (int i = width * 32; i >= 10; i--)
  // {
  //   if (i <= width * 32 - 20)
  //   {
  //     dmd.drawString(((width * 32) + i) - ((width * 32) - 20), 8, "NEW");
  //   }
  //   dmd.drawString(i, 26, "ZEALAND");
  //   delay_engine(20);
  // }
  for (int i = width * 32; i >= 10; i--)
  {
    if (i >= 32)
    {
      dmd.drawString(i, 8, "NEW");
    }
    dmd.drawString(i, 26, "ZEALAND");
    delay_engine(20);
  }
  delay_engine(9000);
  for (int i = 8; i > -40; i--)
  {
    dmd.clearScreen();
    if (i > -10)
    {
      dmd.drawString(32, i, "NEW");
    }
    if (i > -36)
    {
      dmd.drawString(10, i - 8 + 26, "ZEALAND");
    }
    motion.skyTower(5, i - 8 + 48);
    delay_engine(30);
  }
  delay_engine(5000);
  motion.cleaningCurtain();
  takeOff();
}

void usa()
{

  delay_engine(1000);
  for (int i = width * 32; i >= 35; i--)
  {
    dmd.drawString(i, 16, "USA");
    delay_engine(20);
  }
  delay_engine(9000);
  for (int i = 16; i > -32; i--)
  {
    dmd.clearScreen();
    if (i > -10)
    {
      dmd.drawString(35, i, "USA");
    }

    motion.statueOfLiberty(32, i - 16 + 48);
    delay_engine(30);
  }
  delay_engine(5000);
  motion.cleaningCurtain();
  takeOff();
}

void takeOff()
{
  int count = 0;
  int b = 0;
  for (int a = -60; a < 60; a++)
  {
    if (count == 0)
    {
      count = 2;
      b = -a;
    }
    motion.aeroplane(a - 38, b);
    delay_engine(20);
    count--;
    dmd.clearScreen();
  }
}

// void flag()
// {
//   for (uint8_t i = 0; i < 10; i++)
//   {
//     motion.flagStill(0, 0);
//     delay_engine(150);
//     dmd.clearScreen();
//     motion.flagShift(0, 0);
//     delay_engine(150);
//     dmd.clearScreen();
//     motion.flagRoll(0, 0);
//     delay_engine(150);
//     dmd.clearScreen();
//   }
// }

void loop()
{
  startAnimation();
  dmd.clearScreen();
  displayBold();
}