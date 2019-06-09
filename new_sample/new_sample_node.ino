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

bool h12;
bool PM;
bool Century = false;
bool time_state;
uint16_t offset_address = 0;
uint16_t message_count = 0;
uint16_t actual_addr = 0;

//const String startConfigMsg = "sc";
const char *ssidAP = "TESTNODE";
const char *passwordAP = "TEST123123";
const uint8_t *FONTEng = SystemFont5x7;
const uint8_t *FONTBold = Arial_Black_16;
const uint8_t width = 1;
const uint8_t height = 1;
String Message = "";
const String month_array[12] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
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
DS3231 Clock;

void setup()
{
    Wire.begin();
    Serial.begin(9600);
    WiFi.softAP(ssidAP, passwordAP);
    delay(500);
    server.on("/", handleRoot);
    server.on("/ff68d3ae34a775477225b8a091626765", handleQueryArgs);
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
    else if (key == "T")
    {
        String time_var = "->" + value + "|";
        WriteDateTimeIntoRTC(time_var);
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

String get_time(String seperator)
{
    String Time = "";
    int hr = (Clock.getHour(h12, PM) % 12) == 0 ? 12 : (Clock.getHour(h12, PM) % 12); //if else
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

String get_date(String seperator)
{
    String valuee_date = String(Clock.getDate(), DEC);
    if (Clock.getDate() < 10)
        valuee_date = "0" + valuee_date;
    String valuee_month = month_array[Clock.getMonth(Century) - 1];
    String valuee_year = String(Clock.getYear(), DEC);
    if (Clock.getYear() < 10)
        valuee_year = "0" + valuee_year;
    valuee_year = "20" + valuee_year;
    String Time = valuee_month + valuee_date;
    return Time;
}
String get_Temp()
{
    float temperature = Clock.getTemperature();
    //  Serial.println(temperature);
    String temp = String(temperature);
    temp = temp.substring(0, temp.length() - 1);
    if (temperature < 10)
        temp = "0" + temp;
    return temp;
}
void mid_display_nepfont(int y_coordinate, int x_old, String message, int view_window_size)
{
    int x_coordinate = (((view_window_size)-message.length() * 6 + 5) / 2) + x_old;
    dmd.drawString(x_coordinate, y_coordinate, message);
}

void mid_display_engfont(int y_coordinate, int x_old, String message, int view_window_size)
{
    int x_coordinate = (((view_window_size)-message.length() * 6) / 2) + x_old;
    dmd.drawString(x_coordinate, y_coordinate, message);
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

void WriteDateTimeIntoRTC(String input)
{
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
    // dmd.drawString(0,0,get_time(":"));
    // dmd.drawString(0,8,get_date(","));
    delay_engine(500);
    mid_display_engfont(8, 0, get_time(" "), 32);
    delay_engine(500);
}
void show_temp()
{
    dmd.selectFont(FONTEng);
    //  dmd.drawString(0, 0, 'temp');
    mid_display_engfont(8, -4, get_Temp(), 32);
    dmd.setPixel(24, 8, GRAPHICS_ON);
    dmd.setPixel(24, 9, GRAPHICS_ON);
    dmd.setPixel(25, 8, GRAPHICS_ON);
    dmd.setPixel(25, 9, GRAPHICS_ON);
    dmd.drawString(32 - 5, 8, "C");
    dmd.drawString(3, 0, "TEMP:");
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
        dmd.drawString(i, 1, Message);
        delay_engine(30);
    }
    Message = Message.substring(1, string_length);
    string_length = Message.length();
    int loop_length = string_length;
    for (int i = 0; i < loop_length; i++)
    {
        string_length = Message.length();
        buffer_char = Message[0];
        char_ascii_num = int(buffer_char);
        over_scroll_length = bold_array[char_ascii_num - 32];

        for (int j = 0; j > -(over_scroll_length + 1); j--)
        {
            dmd.drawString(j, 1, Message);
            delay_engine(30);
        }
        Message = Message.substring(1, string_length);
    }
}
void loop()
{
    for (int i = 0; i <= 5; i++)
    {
        show_date_time();
    }
    dmd.clearScreen();
    displayBold();
    dmd.clearScreen();
    for (int i = 0; i <= 5; i++)
    {
        show_temp();
    }
    dmd.clearScreen();
    motion.Smiley(6, 0);
    delay_engine(3000);
    dmd.clearScreen();
    //  motion.sad(0,0);
    //  delay_engine(3000);
    //  dmd.clearScreen();
    motion.Love(6, 0);
    delay_engine(1000);
    dmd.clearScreen();
    motion.Thumbs_Up(6, 0);
    delay_engine(1000);
    dmd.clearScreen();
    motion.Vass(6, 0);
    delay_engine(1000);
    dmd.clearScreen();
    motion.Ganesh(6, 0);
    delay_engine(1000);
    dmd.clearScreen();
    motion.swastikSign(6, 0);
    delay_engine(1000);
    dmd.clearScreen();
    motion.Stop_Sign(6, 0);
    delay_engine(1000);
    dmd.clearScreen();
    motion.cup(6, 0);
    delay_engine(1000);
    dmd.clearScreen();
    dmd.clearScreen();
}