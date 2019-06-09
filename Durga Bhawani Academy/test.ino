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

//Wifi
//const String startConfigMsg = "sc";
const char* ssidAP = "DBASchool";
const char* passwordAP = "DBASchool";

ESP8266WebServer server(80);

DS3231 Clock;

bool h12;
bool PM;
bool Century = false;
bool time_state;
uint16_t offset_address = 0;
uint16_t message_count = 0;
uint16_t actual_addr = 0;
String Message = "";


const int width PROGMEM = 5;
const uint8_t *FONTNep = NepFont5x7;
const uint8_t *FONTEng = SystemFont5x7;
const String month_array[12] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
const String greeting_array[4] = {"Good Morning", "Good Afternoon", "Good Evening", "Good Night"};

SPIDMD dmd(width, 1);

void setup()
{
    Serial.begin(9600);
    Wire.begin();
    server.on("/", handleRoot);
    server.on("/val", handleQueryArgs);
    server.onNotFound(handleNotFound);
    server.begin();
    dmd.begin();
    EEPROM.begin(512);
    dmd.selectFont(FONTEng);
    dmd.setBrightness(255);
}

void handleRoot(){
    server.send(200, "text/plain", "hello from server");
}

void handleQueryArgs() {   //Handler
  String key = server.argName(0);
  String value = server.arg(0);
  if(key == "M"){
      Message = value;
      write_to_EEPROM(0);
  }else if(key == "T"){
    String time_var = "->" + value + "|";
    WriteDateTimeIntoRTC(time_var);
  }
  server.send(200, "text/plain", "i heard you");
}

void handleNotFound(){  
  server.send(404, "text/plain", "Error 404! Incorrect Path");
}

void delay_engine(int delay_val)
{
  unsigned long startMs = millis();
  while (millis() - startMs <= delay_val) {
    server.handleClient();
    delay(1);
  }
}

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

void write_to_EEPROM(uint16_t address)
{
  uint16_t value_len = Message.length();
  uint16_t j = 0;
  if(value_len <= 255){
    EEPROM.write(0, value_len);
    EEPROM.commit();
    EEPROM.write(1, 0);
    EEPROM.commit();
  }else if(value_len > 255){
    uint8_t second_value_len = value_len - 255;
    EEPROM.write(0, 255);
    EEPROM.commit();
    EEPROM.write(1, second_value_len);
    EEPROM.commit();
  }
  EEPROM.write(address, value_len);
  for (uint16_t i = address; i < (address + value_len); i++)
  {
    char temp_char = Message[j];
    EEPROM.write(i + 2, temp_char);
    EEPROM.commit();
    j++;
  }
  Serial.println("EEPROM writing");
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

void display_message()
{
  int count = 0;
  int trigger_right = 0;
  int trigger_left = width * 32;
  eeprom_read();
  uint16_t string_length = Message.length();
  for (int i = width * 32; i > -6; i--)
  {
    dmd.drawString(i, 4, Message);
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
  Message = Message.substring(1, string_length);
  int loop_length = string_length;
  for (int i = 0 ; i < loop_length; i++)
  {
    string_length = Message.length();

    for (int j = 0; j > -6; j--)
    {
      dmd.drawString(j, 4, Message);
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
    Message = Message.substring(1, string_length);
  }

  actual_addr = 0;

}


void displayConstMessage(String const_Message)
{
  uint16_t string_length = const_Message.length();
  for (int i = width * 32; i > -6; i--)
  {
    dmd.drawString(i, 4, const_Message);
    delay_engine(20);
  }
  const_Message = const_Message.substring(1, string_length);
  int loop_length = string_length;
  for (int i = 0 ; i < loop_length; i++)
  {
    string_length = const_Message.length();

    for (int j = 0; j > -6; j--)
    {
      dmd.drawString(j, 4, const_Message);
      delay_engine(20);
    }
    const_Message = const_Message.substring(1, string_length);
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



void fading_effect() {

  for (int j = 0; j < 2; j++)
  {
    for (int i = 255; i > 50; i--)
    {
      dmd.setBrightness(i);
      delay_engine(20);
    }
    for (int i = 50; i < 255; i++)
    {
      dmd.setBrightness(i);
      delay_engine(20);
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



void loop(){

 displayConstMessage("Durga Bhawani Academy");
 Serial.println("first_stage passed");
 dmd.clearScreen();
 dmd.selectFont(Arial_Black_16);
 dmd.drawString(32, 1, "DBA school");
 delay_engine(500);
 Serial.println("2_stage passed");
 dmd.drawString(32, 1, "          ");
 delay_engine(200);
 Serial.println("3_stage passed");
 dmd.drawString(32, 1, "DBA school");
 delay_engine(500);
 dmd.drawString(32, 1, "          ");
 delay_engine(200);
 dmd.drawString(32, 1, "DBA school");
 fading_effect();
 Serial.println("4_stage passed");
 delay_engine(1000);
 dmd.selectFont(FONTEng);
 dmd.clearScreen();
 for (int i = 0; i < 5; i++)
  {
     greeting();
     show_date_time();
  }
  Serial.println("5_stage passed");
  cleaning_boarder_LR();
  dmd.clearScreen();
  dmd.selectFont(FONTEng);
  Serial.println("6_stage passed");
  display_message();
  Serial.println("7_stage passed");
  cleaning_boarder_LR();
  dmd.clearScreen();
  //eeprom_read();

}