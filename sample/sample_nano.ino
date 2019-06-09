
#include <EEPROM.h>
#include <DS3231.h>
#include <Wire.h>
#include <SPI.h>
#include <DMD2.h>
//#include <DMD_Nepali.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/NepFont5x7.h>
#include <fonts/Arial_Black_16.h>
#include <animation.h>


DS3231 Clock;

bool h12;
bool PM;
bool Century = false;
bool time_state;
uint16_t offset_address = 0;
uint16_t message_count = 0;
uint16_t actual_addr = 0;


const int width PROGMEM = 1;
const uint8_t *FONTNep = NepFont5x7;
const uint8_t *FONTEng = SystemFont5x7;
const String month_array[12] PROGMEM = {"JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE", "JULY", "AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER"};
const String greeting_array[4] PROGMEM = {"Good Morning", "Good Afternoon", "Good Evening", "Good Night"};

SoftDMD dmd(width, 1); // DMD controls the entire display
//DMD_Nepali nep(dmd);


void CheckSerialInput()
{
  if (Serial.available()) {
    String serialInput = Serial.readString();
    Serial.flush();
    Serial.println(serialInput);
    getkeyandvalues(serialInput);
  }
}

void getkeyandvalues(String input)
{Serial.println("i am inside getkeyandvalues value is ====> " + input);
  String sub = input.substring(0, 1);
  if (sub == "M" || sub == "-") {
    if (sub == "M") {
      offset_address = 0;
      message_count = 1;
      DecodeAndSaveMessage(input);
    }
    if (sub == "-") {
      message_count++;
      DecodeAndSaveMessage(input);
      
    }
  } else if (sub == "T") {
    WriteDateTimeIntoRTC(input);
  } else if (sub == "B") {
    DecodeAndSaveBrightness(input);
  }
}

void DecodeAndSaveBrightness(String In_val)
{
  uint16_t startindex = In_val.indexOf("->");
  uint16_t endindex = In_val.indexOf("|");
  int screen_brightness = In_val.substring(startindex + 2, endindex).toInt();
  EEPROM.write(511, screen_brightness);
}

void let_brightness()
{
  int brightness = EEPROM.read(511);
  dmd.setBrightness(brightness);
}

void DecodeAndSaveMessage(String In_val)
{
  uint16_t location_feedback = 0;
  uint16_t startindex = In_val.indexOf("->");
  uint16_t endindex = In_val.indexOf("|");
  String msg = In_val.substring(startindex + 2, endindex);
  Serial.println("i am at DecodeAndSaveMessage ===> " + msg);
  offset_address = write_to_EEPROM(msg, offset_address);
}

uint16_t write_to_EEPROM(String value, uint16_t address)
{
  Serial.println("i am at write eeprom and string received is ====> " + value);
  Serial.println("current address ==> " + String(address, DEC));
  uint16_t value_len = value.length();
  Serial.println("current length of value ==> " + String(value_len, DEC));
  uint16_t j = 0;
  EEPROM.write(address, value_len);
  for (uint16_t i = address; i < (address + value_len); i++)
  {
    char temp_char = value[j];
    EEPROM.write(i + 1, temp_char);
    j++;
  }
  Serial.println("message count =====> " + String(message_count, DEC));
  EEPROM.write(510, message_count);
  address = address + value_len + 1;
  return address;
  
}


String eeprom_read(int label)
{
  String notice_val = "";
  int x = EEPROM.read(label);
  for (int i = label; i < (x + label); i++)
  {
    char bufferr = EEPROM.read(i + 1);
    notice_val = String(notice_val + bufferr);
  }
  Serial.println("eeprom called, value taken =====> " + notice_val);
  actual_addr = label + x + 1;
  return notice_val;
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


void setup()
{
  Wire.begin();
  Serial.begin(9600);
  // let_brightness();
  //dmd.selectFont(FONTNep);
  dmd.selectFont(FONTEng);
  dmd.setBrightness(255);
  dmd.begin();

  delay_engine(100);

}

void loop()
{ 
  
}