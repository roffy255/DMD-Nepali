#include <EEPROM.h>
#include <Wire.h>
#include <SPI.h>
#include <DMD2.h>
#include <DMD_Nepali.h>
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
uint16_t msg_count = 0;
uint16_t actual_addr = 0;

//const String startConfigMsg = "sc";
const char *ssidAP = "SPublication";
const char *passwordAP = "SanPublication123";
const uint8_t *FONTNep = NepFont5x7;
const uint8_t *FONTEng = SystemFont5x7;
const uint8_t *FONTBold = Arial_Black_16;
const uint8_t width = 5;
const uint8_t height = 1;
String msg = "";
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

const uint8_t nep_array[93] = {
    2, 8, 8, 3, 0,
    8, 10, 6, 6, 7, 10, 9, 0, 0, 0,
    6, 6, 0, 0, 11, 11, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 7, 8, 7,
    8, 8, 8, 6, 6, 8, 8, 6, 7, 8,
    7, 8, 7, 8, 7, 8, 10, 2, 8, 8,
    8, 8, 8, 2, 1, 8, 8, 8, 9, 0,
    8, 8, 8, 9, 0, 11, 3, 8, 6, 6,
    6, 7, 8, 6, 7, 8, 7, 6};

ESP8266WebServer server(80);

SPIDMD dmd(width, height);

DMD_Nepali nep(dmd);

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  WiFi.softAP(ssidAP, passwordAP);
  delay(500);
  server.on("/", handleRoot);
  server.on("/sanjiwani", handleQueryArgs);
  server.onNotFound(handleNotFound);
  server.begin();
  delay(500);
  dmd.begin();
  EEPROM.begin(512);
  dmd.selectFont(FONTEng);
  dmd.setBrightness(255);
}

void handleRoot()
{
  server.send(200, "text/plain", "hello from server");
}

void handleQueryArgs()
{ //Handler
  String key = server.argName(0);
  if (key == "M")
  {
    msg = server.arg(0);
    write_to_EEPROM(0);
  }
  else if (key == "T")
  {
    String time_var = "->" + server.arg(0) + "|";
  }
  else if (key == "B")
  {
    EEPROM.write(510, server.arg(0).toInt());
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
  uint16_t value_len = msg.length();
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
    char temp_char = msg[j];
    EEPROM.write(i + 2, temp_char);
    EEPROM.commit();
    j++;
  }
  Serial.println("EEPROM writing");
}

void eeprom_read()
{
  msg = "";
  int x = EEPROM.read(0) + EEPROM.read(1);
  for (int i = 0; i < (x); i++)
  {
    char bufferr = EEPROM.read(i + 2);
    msg = String(msg + bufferr);
  }
  Serial.println(msg);
}

void checkAndDisplay()
{
  eeprom_read();
  if (msg[0] == '~')
  {
    displayNepali();
  }
  else if (msg[0] == '}')
  {
    display_msg();
  }
}

void display_msg()
{
  
  msg = msg.substring(1, msg.length());
  uint16_t string_length = msg.length();
  for (int i = width * 32; i > -6; i--)
  {
    dmd.drawString(i, 4, msg);
    delay_engine(20);
  }
  msg = msg.substring(1, string_length);
  int loop_length = string_length;
  for (int i = 0; i < loop_length; i++)
  {
    string_length = msg.length();

    for (int j = 0; j > -6; j--)
    {
      dmd.drawString(j, 4, msg);
      
      delay_engine(20);
      
    }
   
    msg = msg.substring(1, string_length);
  }

  actual_addr = 0;
}

void displayConstmsg(String const_msg)
{
  uint16_t string_length = const_msg.length();
  for (int i = width * 32; i > -6; i--)
  {
    dmd.drawString(i, 4, const_msg);
    delay_engine(20);
  }
  const_msg = const_msg.substring(1, string_length);
  int loop_length = string_length;
  for (int i = 0; i < loop_length; i++)
  {
    string_length = const_msg.length();

    for (int j = 0; j > -6; j--)
    {
      dmd.drawString(j, 4, const_msg);
      delay_engine(20);
    }
    const_msg = const_msg.substring(1, string_length);
  }
}

//void displayBold()
//{
//  dmd.selectFont(Arial_Black_16);
//  msg = msg.substring(1,msg.length());
//  uint16_t string_length = msg.length();
//  char buffer_char = msg[0];
//  int char_ascii_num = int(buffer_char);
//  int over_scroll_length = bold_array[char_ascii_num - 32];
//  for (int i = width * 32; i > -(over_scroll_length + 1); i--)
//  {
//    dmd.drawString(i, 1, msg);
//    delay_engine(30);
//    Serial.println(msg);
//    Serial.println(char_ascii_num);
//  }
//  msg = msg.substring(1, string_length);
//  int loop_length = string_length;
//  for (int i = 0; i < loop_length; i++)
//  {
//    string_length = msg.length();
//    buffer_char = msg[0];
//    char_ascii_num = int(buffer_char);
//    over_scroll_length = bold_array[char_ascii_num - 32];
//
//    for (int j = 0; j > -(over_scroll_length + 1); j--)
//    {
//      dmd.drawString(j, 1, msg);
//      delay_engine(30);
//    }
//    msg = msg.substring(1, string_length);
//    Serial.println(msg);
//    Serial.println(char_ascii_num);
//  }
//  dmd.selectFont(FONTEng);
//}

void displayNepali()
{
  msg = msg + " ";
  uint16_t msgLength = msg.length();
  msg = msg.substring(1, msgLength);
  char buffer_char = msg[0];
  uint8_t char_ascii_num = int(buffer_char);
  uint16_t over_scroll_length;
  uint8_t cutter;

  /***********************first stage checking**************************/
  if (msg[1] == 'c' || msg[1] == 'g' || msg[1] == 'h' || msg[1] == 'a' || msg[1] == 'b' || msg[1] == '"' || msg[1] == '!')
  {
    over_scroll_length = nep_array[char_ascii_num - 32];
    cutter = 2;
  }
  else if (msg[1] == 'l' || msg[1] == '_' || msg[1] == 'k' || msg[1] == '^' || msg[1] == '`')
  {
    over_scroll_length = nep_array[char_ascii_num - 32] + 2;
    cutter = 2;
  }
  else if (msg[1] == 'm' & (msg[0] != '}' & msg[0] != '5' & msg[0] != '[' & msg[0] != 'B' & msg[0] != '?' & msg[0] != '@') & msg.length() > 2)
  {
    over_scroll_length = nep_array[int(msg[2]) - 32] + (nep_array[char_ascii_num - 32] - 2);
    cutter = 3;
    if (msg[3] == 'l' || msg[3] == '_' || msg[3] == 'k' || msg[3] == '^' || msg[3] == '`')
    {
      over_scroll_length = nep_array[int(msg[2]) - 32] + (nep_array[char_ascii_num - 32] - 2) + 2;
      cutter = 4;
    }
    else if (msg[3] == 'c' || msg[3] == 'g' || msg[3] == 'h' || msg[3] == 'a' || msg[3] == 'b' || msg[3] == '"' || msg[3] == '!')
    {
      over_scroll_length = nep_array[int(msg[2]) - 32] + (nep_array[char_ascii_num - 32] - 2);
      cutter = 4;
    }
  }
  else if (msg[1] == 'm' & (msg[0] == '}' || msg[0] == '5' || msg[0] == '[' || msg[0] == 'B' || msg[0] == '?' || msg[0] == '@'))
  {
    over_scroll_length = nep_array[char_ascii_num - 32];
    cutter = 2;
    Serial.println("i am on");
  }
  else if (msg[1] == 'm' & (msg[0] != '}' & msg[0] != '5' & msg[0] != '[' & msg[0] != 'B' & msg[0] != '?' & msg[0] != '@') & msg.length() <= 2)
  {
    over_scroll_length = nep_array[char_ascii_num - 32];
    cutter = 2;
  }
  else
  {
    over_scroll_length = nep_array[char_ascii_num - 32];
    cutter = 1;
  }

  for (int i = width * 32; i > -(over_scroll_length); i--)
  {
    nep.drawString(i, 0, "~" + msg);
    delay_engine(30);
  }

  /******************************second stage*********************/

  msg = msg.substring(cutter, msg.length());
  int loop_length = msg.length();
  for (int i = 0; i < loop_length;)
  {
    buffer_char = msg[0];
    char_ascii_num = int(buffer_char);

    if (msg[1] == 'c' || msg[1] == 'g' || msg[1] == 'h' || msg[1] == 'a' || msg[1] == 'b' || msg[1] == '"' || msg[1] == '!')
    {
      over_scroll_length = nep_array[char_ascii_num - 32];
      cutter = 2;
    }
    else if (msg[1] == 'l' || msg[1] == '_' || msg[1] == 'k' || msg[1] == '^' || msg[1] == '`')
    {
      over_scroll_length = nep_array[char_ascii_num - 32] + 2;
      cutter = 2;
    }
    else if (msg[1] == 'm' & (msg[0] != '}' & msg[0] != '5' & msg[0] != '[' & msg[0] != 'B' & msg[0] != '?' & msg[0] != '@') & msg.length() > 2)
    {
      over_scroll_length = nep_array[int(msg[2]) - 32] + (nep_array[char_ascii_num - 32] - 2);
      cutter = 3;
      if (msg[3] == 'l' || msg[3] == '_' || msg[3] == 'k' || msg[3] == '^' || msg[3] == '`')
      {
        over_scroll_length = nep_array[int(msg[2]) - 32] + (nep_array[char_ascii_num - 32] - 2) + 2;
        cutter = 4;
      }
      else if (msg[3] == 'c' || msg[3] == 'g' || msg[3] == 'h' || msg[3] == 'a' || msg[3] == 'b' || msg[3] == '"' || msg[3] == '!')
      {
        over_scroll_length = nep_array[int(msg[2]) - 32] + (nep_array[char_ascii_num - 32] - 2);
        cutter = 4;
      }
    }
    else if (msg[1] == 'm' & (msg[0] == '}' || msg[0] == '5' || msg[0] == '[' || msg[0] == 'B' || msg[0] == '?' || msg[0] == '@'))
    {
      over_scroll_length = nep_array[char_ascii_num - 32];
      cutter = 2;
      Serial.println("i am on");
    }
    else if (msg[1] == 'm' & (msg[0] != '}' & msg[0] != '5' & msg[0] != '[' & msg[0] != 'B' & msg[0] != '?' & msg[0] != '@') & msg.length() <= 2)
    {
      over_scroll_length = nep_array[char_ascii_num - 32];
      cutter = 2;
    }
    else
    {
      over_scroll_length = nep_array[char_ascii_num - 32];
      cutter = 1;
    }
    for (int j = 0; j > -(over_scroll_length); j--)
    {
      nep.drawString(j, 0, "~" + msg);
      delay_engine(30);
    }
    msg = msg.substring(cutter, msg.length());
    if (msg.length() == 0)
    {
      break;
    }
  }
  dmd.clearScreen();
  dmd.selectFont(SystemFont5x7);
}

void loop()
{
  checkAndDisplay();
  dmd.selectFont(FONTEng);
  displayConstmsg("Developed by EngineeredYouth. Contact: 9849141984");
}