
#include <EEPROM.h>
#include <DS3231.h>
#include <Wire.h>
#include <SPI.h>
#include <DMD2.h>
#include <DMD_Nepali.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/NepFont1_16bit.h>
#include <fonts/Arial14.h>
#include <fonts/Arial_Black_16.h>
#include <fonts/NepFont5x7.h>

DS3231 Clock;

uint16_t timevar1 = 0, timevar2 = 0;
bool flagTime = true;
uint16_t offset_addr = 0;
bool h12;
bool PM;
bool Century=false;
int width = 1;
bool time_state;

const uint8_t *FONTNep = NepFont5x7;
const uint8_t *FONTEng = SystemFont5x7;
String month_array[12] = {"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};
//const char MESSAGE[] PROGMEM = "सून र चादि पसल";    //Message to be displayed
String name_comp = "Jewellers";
SoftDMD dmd(width,2);  // DMD controls the entire display


String get_time(String seperator){
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
    Time = valuee_hour + ":" + valuee_minute;
     if(Clock.getHour(h12,PM) >= 12)
    { 
      time_state = true;
    }
    else
    {
      time_state = false;
    }
   
  Serial.println(Clock.getHour(h12,PM), DEC);
  return Time;
}

String get_date(String seperator){
  String valuee_date = String(Clock.getDate(), DEC);
  if (Clock.getDate() < 10)
    valuee_date = "0" + valuee_date;
  String valuee_month = month_array[Clock.getMonth(Century)-1];
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
  while(millis() - startMs <= delay_val) {
    CheckSerialInput();
    delay(1);
  }
}


void CheckSerialInput() 
{
  if (Serial.available()){
    String serialInput = Serial.readString();
    Serial.flush();
    Serial.println(serialInput);
    getkeyandvalues(serialInput);
    }
}

void getkeyandvalues(String input)
{
  String sub = input.substring(0,5); 
  if(sub=="price"){
    DecodeAndSavePrice(input);
  } else if(sub=="date_"){
    WriteDateTimeIntoRTC(input);    
  }
}

void DecodeAndSavePrice(String In_val)
{
  uint16_t location_feedback = 0;
  uint16_t startindex = In_val.indexOf("->");
  uint16_t endindex = In_val.indexOf("~");
  String T_gold_price = In_val.substring(startindex+2, endindex);
  In_val = In_val.substring(endindex+1, In_val.length());
  startindex = In_val.indexOf("->");
  endindex = In_val.indexOf("~");
  String F_gold_price = In_val.substring(startindex+2, endindex);
  In_val = In_val.substring(endindex+1, In_val.length());
  startindex = In_val.indexOf("->");
  endindex = In_val.indexOf("~");
  String Silver_price = In_val.substring(startindex+2, endindex);
  Serial.println(T_gold_price);
  Serial.println(F_gold_price);
  Serial.println(Silver_price);
  location_feedback = write_to_EEPROM(T_gold_price, location_feedback);
  location_feedback = write_to_EEPROM(F_gold_price, location_feedback);
  location_feedback = write_to_EEPROM(Silver_price, location_feedback);
  
}

uint16_t write_to_EEPROM(String value, uint16_t address)
{
   uint16_t value_len = value.length();
   uint16_t j = 0;
   EEPROM.write(address, value_len);
   for(uint16_t i = address; i < (address+value_len); i++)
   {
    char temp_char = value[j];
    EEPROM.write(i+1, temp_char);
    j++;
   }
   address = address + value_len + 1;
   return address;
}

void WriteDateTimeIntoRTC(String input){
  uint16_t minuteVal, hr, sec;
  uint16_t date, mth, yr;
  uint16_t startindex = input.indexOf("->");
  uint16_t endindex = input.indexOf("~");
  
  date = (input.substring(startindex+2,startindex+4)).toInt();
  mth = (input.substring(startindex+5,startindex+7)).toInt();
  yr = (input.substring(startindex+8,startindex+10)).toInt();
  hr = (input.substring(startindex+11,startindex+13)).toInt();
  minuteVal = (input.substring(startindex+14,startindex+16)).toInt();
  sec = (input.substring(startindex+17,startindex+19)).toInt();

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
  dmd.selectFont(SystemFont5x7);
  dmd.drawString(1,24,get_time(":"));
  dmd.drawString(1,16,get_date(","));
  if(time_state == true)
  {
    dmd.drawString(50, 8, "pm");
  }

  else
  {
    dmd.drawString(50, 8 , "am");
  }
////  dmd.drawString(33,4, name_of_comp);
//  delay_engine(500);
//   dmd.drawString(5,8,get_time(":"));
//  
//  delay_engine(500);
}


String eeprom_read(int label)
{
  
  String message="";
  int x = EEPROM.read(label);
  for(int i=label; i<(x+label); i++)
  {
    char bufferr=EEPROM.read(i+1);
    message = String(message + bufferr);
  }
  offset_addr = label + x + 1;
  return message;
}


void show_price_update_nep()
{
    String tgold = eeprom_read(offset_addr);
    String fgold = eeprom_read(offset_addr);
    String silver = eeprom_read(offset_addr);
    offset_addr = 0;
    Serial.println(tgold);
    Serial.println(fgold);
    Serial.println(silver);
    dmd.selectFont(FONTNep);
  dmd.drawString(8, 0, "]");
  dmd.drawString(2, 8, fgold);
  
  dmd.drawString(6, 16, "_");
  dmd.drawString(7, 24, silver);
 
}

void show_price_update_eng()
{
   String tgold = eeprom_read(offset_addr);
    String fgold = eeprom_read(offset_addr);
    String silver = eeprom_read(offset_addr);
    offset_addr = 0;
    Serial.println(tgold);
    Serial.println(fgold);
    Serial.println(silver);
    dmd.selectFont(FONTEng);
  dmd.drawString(5,0, "Gold");
  dmd.drawString(1,8, fgold);
  
  dmd.selectFont(FONTNep);
  dmd.drawString(3, 16, "a");
  dmd.selectFont(FONTEng);
  dmd.drawString(7, 24, silver);
  
}

void display_company()
{
  dmd.selectFont(FONTEng);
  int char_len = name_comp.length();
   for(int i = width*32; i>=-22 ; i--)
  {
    dmd.drawString(i,4,name_comp);
    Serial.println(i);
    delay_engine(80);
    //dmd.clearScreen();
  }
}



void setup() 
{
  Wire.begin();
  Serial.begin(9600);
  dmd.setBrightness(255);
  dmd.selectFont(FONTNep);
  dmd.begin();
  delay_engine(100);

}

void loop() 
{
dmd.drawString(0,0, "HELLO");

// show_date_time();
// display_company();
// delay_engine(2000);

// dmd.clearScreen();


// for(int i = 0; i<=100; i++)
// {
//  show_price_update_nep();
//  delay_engine(i);
// }

// dmd.clearScreen();

// for(int i = 0; i<=100; i++)
// {
//     show_price_update_eng();
//     delay_engine(i);
// }

// dmd.clearScreen();
//  timevar1 = millis();
//  if(flagTime)
//  {
//     show_price_update_nep();
    
    
//     if((timevar1 - timevar2) >= 5000)
//     {

//       flagTime = false;
//       timevar2 = timevar1;
//       dmd.clearScreen();
//     }

    
//  }

//     else
//     {
//        show_price_update_eng();
//       display_company();
//      dmd.clearScreen();
//       flagTime = true;
//       timevar2= millis();
//     }

}
