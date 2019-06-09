
#include <EEPROM.h>
#include <DS3231.h>
#include <Wire.h>
#include <SPI.h>
#include <DMD2.h>
#include <DMD_Nepali.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/NepFont5x7.h>
#include <fonts/Arial_Black_16.h>



//----------------------------------------------------flags and variables globally defined------------------------------------//

bool h12;
bool PM;
bool Century = false;
bool time_state;
uint16_t offset_address = 0;
uint16_t message_count = 0;
uint16_t actual_addr = 0;

//-----------------------------------------------------constants declared in ROM----------------------------------------------//

const int width PROGMEM = 5;
const uint8_t *FONTNep = NepFont5x7;
const uint8_t *FONTEng = SystemFont5x7;
const String month_array[12] PROGMEM = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
const String greeting_array[4] PROGMEM = {"Good Morning", "Good Afternoon", "Good Evening", "Good Night"};
const uint8_t font_size_nepali[92] PROGMEM = {
    8, 8, 3, 0, 8, 10, 6, 6, 7, 10,
    9, 0, 0, 0, 6, 6, 0, 0, 11, 11,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 7, 8, 7, 8, 8, 8, 6, 6, 8,
    8, 6, 7, 8, 7, 8, 7, 8, 7, 8,
    10, 2, 8, 8, 8, 8, 8, 2, 1, 8,
    8, 8, 0, 0, 8, 8, 8, 9, 0, 11,
    3, 8, 6, 6, 6, 7, 8, 6, 7, 8,
    7, 6
};


//-----------------------------------------------------creating an object-----------------------------------------------------//

DS3231 Clock;
SoftDMD dmd(width, 1); 
DMD_Nepali nep(dmd);


String get_time(String seperator) {
  String Time = "";
  int hr = (Clock.getHour(h12, PM) % 12) == 0 ? 12 : (Clock.getHour(h12, PM) % 12);     //if else
  String valuee_hour = String(hr, DEC);
  if (hr < 10)
    valuee_hour = "0" + valuee_hour;
  String valuee_minute = String(Clock.getMinute(), DEC);
  if (Clock.getMinute() < 10)
    valuee_minute = "0" + valuee_minute;
  String valuee_second = String(Clock.getSecond(), DEC);
  if (Clock.getSecond() < 10)
    valuee_second = "0" + valuee_second;
  Time = valuee_hour + seperator + valuee_minute;
  if (Clock.getHour(h12, PM) >= 12)
  {
    time_state = true;
  }
  else
  {
    time_state = false;
  }

  Serial.println(Clock.getHour(h12, PM), DEC);
  return Time;
}

String get_date(String seperator) {
  String valuee_date = String(Clock.getDate(), DEC);
  if (Clock.getDate() < 10)
    valuee_date = "0" + valuee_date;
  String valuee_month = month_array[Clock.getMonth(Century) - 1];
  String valuee_year = String(Clock.getYear(), DEC);
  if (Clock.getYear() < 10)
    valuee_year = "0" + valuee_year;
  valuee_year = "20" + valuee_year;
  String Time =  valuee_month + valuee_date;
  return Time;
}

void delay_engine(int delay_val)
{
  unsigned long startMs = millis();
  while (millis() - startMs <= delay_val) {
    CheckSerialInput();
    //let_brightness();
    delay(1);
  }
}

void mid_display_nepfont(int y_coordinate, int x_old, String message, int view_window_size)
{
  int x_coordinate = (((view_window_size) - message.length() * 6 + 5) / 2) + x_old;
  dmd.drawString(x_coordinate, y_coordinate, message);
}

void mid_display_engfont(int y_coordinate, int x_old, String message, int view_window_size)
{
  int x_coordinate = (((view_window_size) - message.length() * 6) / 2) + x_old;
  dmd.drawString(x_coordinate, y_coordinate, message);
}

void CheckSerialInput()
{
  if (Serial.available()) {
    String serialInput = Serial.readString();
    digitalWrite(4, LOW);
    Serial.flush();
    Serial.println(serialInput);
    getkeyandvalues(serialInput);
  }
}

void getkeyandvalues(String input)
{
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
  // Serial.println(msg);
  offset_address = write_to_EEPROM(msg, offset_address);
}

uint16_t write_to_EEPROM(String value, uint16_t address)
{
  uint16_t value_len = value.length();
  uint16_t j = 0;
  EEPROM.write(address, value_len);
  for (uint16_t i = address; i < (address + value_len); i++)
  {
    char temp_char = value[j];
    EEPROM.write(i + 1, temp_char);
    j++;
  }
  EEPROM.write(510, message_count);
  address = address + value_len + 1;
  return address;
  Serial.println(value);
}

void WriteDateTimeIntoRTC(String input) {
  uint16_t minuteVal, hr, sec;
  uint16_t date, mth, yr;
  uint16_t startindex = input.indexOf("->");
  uint16_t endindex = input.indexOf("|");

  date = (input.substring(startindex + 2, startindex + 4)).toInt();
  mth = (input.substring(startindex + 5, startindex + 7)).toInt();
  yr = (input.substring(startindex + 8, startindex + 10)).toInt();
  hr = (input.substring(startindex + 11, startindex + 13)).toInt();
  minuteVal = (input.substring(startindex + 14, startindex + 16)).toInt();
  sec = (input.substring(startindex + 17, startindex + 19)).toInt();

  Serial.println(date);
  Serial.println(mth);
  Serial.println(yr);
  Serial.println(hr);
  Serial.println(minuteVal);
  Serial.println(sec);


  Clock.setHour(hr);
  Clock.setMinute(minuteVal);
  Clock.setSecond(sec);
  Clock.setYear(yr);
  Clock.setMonth(mth);
  Clock.setDate(date);
}


void show_date_time()
{
  dmd.selectFont(FONTEng);
  mid_display_engfont(8, 0, get_time(":"), 32);
  mid_display_engfont(0, 0, get_date(","), 32);
  delay_engine(500);
  mid_display_engfont(8, 0, get_time(" "), 32);
  delay_engine(500);
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
  actual_addr = label + x + 1;
  return notice_val;
}

void display_message()
{
  int count = 0;
  int trigger_right = 0;
  int trigger_left = width * 32;
  int message_length = EEPROM.read(510);
  String notice = eeprom_read(actual_addr);
  uint16_t string_length = notice.length();
  //    animation_whiteline_horizontal(0,0, 32, true, true);
  for (int i = width * 32; i > -6; i--)
  {
    dmd.drawString(i, 4, notice);
    // nep.drawString(i, 0, notice, notice.length());
    snake_boarder_right(trigger_right);
    trigger_right++;
    if (trigger_right > width * 32) {
      trigger_right = 0;
    }

    snake_boarder_left(trigger_left);
    trigger_left--;
    if (trigger_left < 0) {
      trigger_left = width * 32;
    }
    delay_engine(20);
  }
  int loop_length = string_length;
  for (int i = 0 ; i < loop_length; i++)
  {
    string_length = notice.length();

    if (message_length > 1) {

      if (string_length < 30) {
        if (count < message_length) {
          notice = notice + eeprom_read(actual_addr);
          count++;
          string_length = notice.length();
          loop_length = string_length + i;
        }
      }
    }
    notice = notice.substring(1, string_length);
    for (int j = 0; j > -6; j--)
    {
      dmd.drawString(j, 4, notice);
      snake_boarder_right(trigger_right);
      trigger_right++;
      if (trigger_right > width * 32) {
        trigger_right = 0;
      }

      snake_boarder_left(trigger_left);
      trigger_left--;
      if (trigger_left < 0) {
        trigger_left = width * 32;
      }
      delay_engine(20);
    }
  }

  actual_addr = 0;

}


void display_message_bold(String notice)
{
  dmd.selectFont(FONTEng);
  uint16_t string_length = notice.length();
//  char first_char = notice[0];
//  uint8_t first_char_size = (int)first_char;
//  byte sizee = Arial_Black_16[(first_char_size-33)+7];
//  int loop_overflow  = sizee;
//  //    animation_whiteline_horizontal(0,0, 32, true, true);
//  Serial.println(first_char);
//  Serial.println(first_char_size);
//  Serial.println(loop_overflow);
   for (int i = width * 32; i > -6; i--)
  {
    dmd.drawString(i, 4, notice);
    delay_engine(20);
  }
  int loop_length = string_length;
//  notice = notice.substring(1, string_length);
 for (int i = 0 ; i < loop_length; i++)
  {
    string_length = notice.length();
    notice = notice.substring(1, string_length);
//    first_char = notice[0];
//    first_char_size = (int)first_char;
//    sizee = Arial_Black_16[(first_char_size-33)+7];
//    loop_overflow  = sizee;
//  Serial.println(first_char);
//  Serial.println(first_char_size);
//  Serial.println(loop_overflow);
    for (int j = 0; j > -6; j--)
    {
      dmd.drawString(j, 4, notice);
      delay_engine(20);
    }
  }

  dmd.selectFont(FONTEng);
}

void display_message_nepali(String NEP_notice){
   char first_char = NEP_notice[1];
   char second_char = NEP_notice[2];
   uint16_t string_length = notice.length() - 1;
   
   if(second_char == 'g' || second_char == 'a' || second_char == 'm' || second_char == 'h' || second_char == 'c' || second_char == 'b' || second_char == '!' || second_char == '"'){
       string_length--;
   }
    for (int i = width * 32; i > -6; i--)
  {
    dmd.drawString(i, 4, notice);
    // nep.drawString(i, 0, notice, notice.length());
    snake_boarder_right(trigger_right);
    trigger_right++;
    if (trigger_right > width * 32) {
      trigger_right = 0;
    }

    snake_boarder_left(trigger_left);
    trigger_left--;
    if (trigger_left < 0) {
      trigger_left = width * 32;
    }
    delay_engine(20);
  }


  for (int i = 0 ; i < loop_length; i++)
  {
    string_length = notice.length();

    if (message_length > 1) {

      if (string_length < 30) {
        if (count < message_length) {
          notice = notice + eeprom_read(actual_addr);
          count++;
          string_length = notice.length();
          loop_length = string_length + i;
        }
      }
    }
    notice = notice.substring(1, string_length);
    for (int j = 0; j > -6; j--)
    {
      dmd.drawString(j, 4, notice);
      snake_boarder_right(trigger_right);
      trigger_right++;
      if (trigger_right > width * 32) {
        trigger_right = 0;
      }

      snake_boarder_left(trigger_left);
      trigger_left--;
      if (trigger_left < 0) {
        trigger_left = width * 32;
      }
      delay_engine(20);
    }
  }
}


void greeting() {
  int timing = Clock.getHour(h12, PM);
  if (timing >= 11 & timing < 17) {
    mid_display_engfont(4, 32, greeting_array[1] , 128);
    //Sun(32, 0);
    //delay_engine(200);
    //FULL_SUN(32, -8);
  } else if (timing >= 17 & timing < 20) {
    mid_display_engfont(4, 32, greeting_array[2] , 128);
  } else if (timing >= 20 & timing < 4) {
    mid_display_engfont(4, 32, greeting_array[3] , 128);
  } else if (timing >= 4 & timing < 11) {
    mid_display_engfont(4, 32, greeting_array[0] , 128);
  }
}

void moving_boarder()
{
  for (int i = 0; i <= width * 32; i++)
  {
    dmd.setPixel(i, 0, GRAPHICS_ON);
    delay_engine(5);
  }

  for (int i = width * 32; i >= 0; i--)
  {
    dmd.setPixel(i, 15, GRAPHICS_ON);
    delay_engine(5);
  }


  for (int i = 0; i <= width * 32; i++)
  {
    dmd.setPixel(i, 1, GRAPHICS_ON);
    delay_engine(5);
  }

  for (int i = width * 32; i >= 0; i--)
  {
    dmd.setPixel(i, 14, GRAPHICS_ON);
    delay_engine(5);
  }
}

void const_boarder()
{
  for (int i = 0; i <= width * 32; i++)
  {
    dmd.setPixel(i, 0, GRAPHICS_ON);
    dmd.setPixel(i, 1, GRAPHICS_ON);
  }

  for (int i = width * 32; i >= 0; i--)
  {
    dmd.setPixel(i, 15, GRAPHICS_ON);
    dmd.setPixel(i, 14, GRAPHICS_ON);
  }
}


void cleaning_boarder_vertical()
{

  for (int j = 15; j >= 0; j--)
  {
    for (int i = 0; i <= 128; i++)
    {
      dmd.setPixel(i, j, GRAPHICS_ON);
    }
    delay_engine(20);
    for (int i = 0; i <= 128; i++)
    {
      dmd.setPixel(i, j, GRAPHICS_OFF);
    }
  }
}

void cleaning_boarder_horizontal_RTL()
{

  for (int j = 128; j >= 0; j--)
  {
    for (int i = 0; i <= 15; i++)
    {
      dmd.setPixel(j, i, GRAPHICS_ON);
    }
    delay_engine(10);
    for (int i = 0; i <= 15; i++)
    {
      dmd.setPixel(j, i, GRAPHICS_OFF);
    }
  }
}


void cleaning_boarder_horizontal_LTR()
{

  for (int j = 0; j <= 128; j++)
  {
    for (int i = 0; i <= 15; i++)
    {
      dmd.setPixel(j, i, GRAPHICS_ON);
    }
    delay_engine(10);
    for (int i = 0; i <= 15; i++)
    {
      dmd.setPixel(j, i, GRAPHICS_OFF);
    }
  }
}


void cleaning_boarder_LR()
{

  for (int j = 0; j <= width * 32; j++)
  {
    for (int i = 0; i <= 15; i++)
    {
      dmd.setPixel(j, i, GRAPHICS_ON);
      dmd.setPixel((width * 32) - j, i, GRAPHICS_ON);
    }
    delay_engine(10);
    for (int i = 0; i <= 15; i++)
    {
      dmd.setPixel(j, i, GRAPHICS_OFF);
      dmd.setPixel((width * 32) - j, i, GRAPHICS_OFF);
    }
  }

}

void draw_box_with_gap(int x1, int y1, int x2, int y2)
{

  if ( x1 == x2)
  {
    if (y1 > y2)
    {
      for (int i = y1; i > y2; i--)
      {
        if (i % 6 == 0)
        {
          dmd.setPixel(x1, i, GRAPHICS_OFF);
        }
        else
        {
          dmd.setPixel(x1, i, GRAPHICS_ON);
        }
        delay_engine(5);
      }
    }
    else
    {
      for (int i = y1; i < y2; i++)
      {

        if (i % 6 == 0)
        {
          dmd.setPixel(x1, i, GRAPHICS_OFF);
        }
        else
        {
          dmd.setPixel(x1, i, GRAPHICS_ON);
        }
        delay_engine(5);
      }
    }
  }

  if ( y1 == y2)
  {
    if (x1 > x2)
    {
      for (int i = x1; i > x2; i--)
      {
        if (i % 6 == 0)
        {
          dmd.setPixel(x1, i, GRAPHICS_OFF);
        }
        else
        {
          dmd.setPixel(i, y1, GRAPHICS_ON);
        }
        delay_engine(5);
      }
    }
    else
    {
      for (int i = x1; i < x2; i++)
      {
        if (i % 6 == 0)
        {
          dmd.setPixel(x1, i, GRAPHICS_OFF);
        }
        else
        {
          dmd.setPixel(i, y1, GRAPHICS_ON);
        }
        delay_engine(5);
      }
    }
  }

}

void stLineDiagP(int x, int y, int lengthLine)
{
  for (int i = 0; i < lengthLine; i++)
  {
    dmd.setPixel(x + i, y + i, GRAPHICS_ON);
  }
}
void stLineDiagN(int x, int y, int lengthLine)
{
  for (int i = 0; i < lengthLine; i++)
  {
    dmd.setPixel(x + i, y - i, GRAPHICS_ON);
  }
}


void stLineHor(int x, int y, int lengthLine)
{
  for (int i = 0; i < lengthLine; i++)
  {
    dmd.setPixel(x + i, y, GRAPHICS_ON);
  }
}

void stLineVer(int x, int y, int lengthLine)
{
  for (int i = 0; i < lengthLine; i++)
  {
    dmd.setPixel(x, y + i, GRAPHICS_ON);
  }
}

void Sun(int x, int y)
{
  dmd.setPixel(x + 4, y + 4, GRAPHICS_ON);
  dmd.setPixel(x + 5, y + 3, GRAPHICS_ON);
  for (int i = 6; i < 13; i++)
  {
    dmd.setPixel(x + i, y + 2, GRAPHICS_ON);
  }
  dmd.setPixel(x + 13, y + 3, GRAPHICS_ON);
  dmd.setPixel(x + 14, y + 4, GRAPHICS_ON);
  for (int i = 5; i < 12; i++)
  {
    dmd.setPixel(x + 3, y + i, GRAPHICS_ON);

  }
  for (int i = 5; i < 12; i++)
  {
    dmd.setPixel(x + 15, y + i, GRAPHICS_ON);
  }
  dmd.setPixel(x + 4, y + 12, GRAPHICS_ON);
  dmd.setPixel(x + 5, y + 13, GRAPHICS_ON);
  dmd.setPixel(x + 14, y + 12, GRAPHICS_ON);
  dmd.setPixel(x + 13, y + 13, GRAPHICS_ON);
  for (int i = 6; i < 13; i++)
  {
    dmd.setPixel(x + i, y + 14, GRAPHICS_ON);
  }
}

void FULL_SUN(int x, int y)
{
  //  stLineHor(x+0,y+11,22);
  // dmd.setPixel(x+7,y+12, GRAPHICS_ON);
  // dmd.setPixel(x+13,y+12, GRAPHICS_ON);


  stLineHor(x + 9, y + 12, 3);
  stLineHor(x + 8, y + 13, 5);
  stLineHor(x + 7, y + 14, 7);
  stLineHor(x + 7, y + 15, 7);
  stLineHor(x + 7, y + 16, 7);
  stLineHor(x + 9, y + 18, 3);
  stLineHor(x + 8, y + 17, 5);
  //lines
  stLineVer(x + 10, y + 7, 4);
  stLineDiagP(4, 2, 3);
  stLineDiagN(14, 4, 3);
  stLineVer(x + 10, y + 20, 4);
  stLineDiagP(x + 14, y + 12, 3);
  stLineDiagN(x + 4, y + 14, 3);
  stLineHor(x + 2, y + 15, 4);
  stLineHor(x + 15, y + 15, 4);
}


void fading_effect() {

  for (int j = 0; j < 2; j++)
  {
    for (int i = 255; i > 50; i--)
    {
      dmd.setBrightness(i);
      delay_engine(10);
    }
    for (int i = 50; i < 255; i++)
    {
      dmd.setBrightness(i);
      delay_engine(10);
    }


  }

  dmd.setBrightness(255);

}


void snake_boarder_right(int call) {
  int count = 0;
  for (int i = call; i < (width * 32) + call; i++) {
    int root = i;
    count++;
    if (i >= width * 32) {
      root = i - (width * 32);
    }
    if (count <= 4) {
      dmd.setPixel(root, 0, GRAPHICS_ON);
      dmd.setPixel(root, 1, GRAPHICS_ON);

    } else {
      dmd.setPixel(root, 0, GRAPHICS_OFF);
      dmd.setPixel(root, 1, GRAPHICS_OFF);
      if (count == 7) {
        count = 0;
      }
    }

  }
}



void snake_boarder_left(int call) {
  int count = 0;
  for (int i = call; i > call - (width * 32); i--)
  {
    int root = i;
    count++;
    if (i <= 0) {
      root = (width * 32) + i;
    }
    if (count <= 4) {
      dmd.setPixel(root, 15, GRAPHICS_ON);
      dmd.setPixel(root, 14, GRAPHICS_ON);

    } else {
      dmd.setPixel(root, 15, GRAPHICS_OFF);
      dmd.setPixel(root, 14, GRAPHICS_OFF);
      if (count == 7) {
        count = 0;
      }
    }

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
  // mid_display_engfont(4, 0, "New Sunshine English School", 159);
//  // display_message_bold("New Sunshine Secondary School");
 // delay_engine(500);
  // draw_box_with_gap(0,15,0,0);
  // draw_box_with_gap(0,0,159,0);
  // draw_box_with_gap(159,0,159,15);
  // draw_box_without_gap(159,15,0,15);
 // fading_effect();
//  delay_engine(500);
//  dmd.drawString(0, 4, "                           ");
//  delay_engine(200);
//  dmd.drawString(0, 4, "New Sunshine Secondary School");
//  delay_engine(500);
//  dmd.drawString(0, 4, "                           ");
//  delay_engine(200);
//  dmd.drawString(0, 4, "New Sunshine Secondary School");
//  delay_engine(1000);
//  cleaning_boarder_LR();




//   for (int i = 0; i < 5; i++)
//   {
//     greeting();
//     show_date_time();
//   }
//   cleaning_boarder_LR();
//   dmd.clearScreen();
//   // moving_boarder();
//   dmd.selectFont(FONTEng);
//   display_message();
//   cleaning_boarder_LR();
//   dmd.clearScreen();

 nep.drawString(0, 2, "~5k!" );
 
  }