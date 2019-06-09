#include <animation.h>
#include <EEPROM.h>
#include <DS3231.h>
#include <Wire.h>
#include <SPI.h>
#include <DMD2.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/NepFont5x7.h>
#include <fonts/Arial_Black_16.h>
#include <DMD_Nepali.h>

DS3231 Clock;

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
String Message = "";
uint8_t speed;

const uint8_t width = 5;
const uint8_t height = 2;
const uint8_t *FONTEng = SystemFont5x7;
const uint8_t *FONTBold = Arial_Black_16;
const String month_array[12] = {"LhX^6", "<g@", "%X^P", "X^)H", "MFI", "%Xk<", "5^D_m5", "N9mX_P", "JIW", "N^8", "K^7aH", ":hD"};
const String numeric_array[10] = {"~s", "~t", "~u", "~v", "~w", "~x", "~y", "~z", "~{", "~|"};
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

SPIDMD dmd(width, height);
animation motion(dmd, dmd, width, height);
DMD_Nepali nep(dmd);

void setup()
{
    Wire.begin();
    Serial.begin(9600);
    dmd.begin();
    delay(1000);
    EEPROM.begin(512);
    dmd.selectFont(FONTEng);
    dmd.setBrightness(255);
}

void delay_engine(int delay_val)
{
    unsigned long startMs = millis();
    while (millis() - startMs <= delay_val)
    {

        CheckSerialInput();
        set_brightness();
        delay(1);
    }
}

void CheckSerialInput()
{
    if (Serial.available())
    {
        String serialInput = Serial.readString();
        Serial.flush();
        Serial.println(serialInput);
        getkeyandvalues(serialInput);
    }
}

void getkeyandvalues(String input)
{
    String sub = input.substring(0, 1);
    if (sub == "T")
    {
        WriteDateTimeIntoRTC(input);
    }
    else if (sub == "B")
    {
        DecodeAndSaveBrightness(input);
    }
}

void DecodeAndSaveBrightness(String In_val)
{
    uint16_t startindex = In_val.indexOf("->");
    uint16_t endindex = In_val.indexOf("|");
    int screen_brightness = In_val.substring(startindex + 2, endindex).toInt();
    EEPROM.write(510, screen_brightness);
    EEPROM.commit();
}

void set_brightness()
{
    dmd.setBrightness(EEPROM.read(510));
}

String get_time(String seperator)
{
    String Time = "";
    int hr = (Clock.getHour(h12, PM) % 12) == 0 ? 12 : (Clock.getHour(h12, PM) % 12); //if else
    String valuee_hour = decode_date(String(hr, DEC));
    if (hr < 10)
        valuee_hour = "s" + valuee_hour;
    String valuee_minute = decode_date(String(Clock.getMinute(), DEC));
    if (Clock.getMinute() < 10)
        valuee_minute = "s" + valuee_minute;
    String valuee_second = String(Clock.getSecond(), DEC);
    if (Clock.getSecond() < 10)
        valuee_second = "0" + valuee_second;
    Time = "~" + valuee_hour + seperator + valuee_minute;
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

String get_date()
{
    String valuee_date = decode_date(String(Clock.getDate(), DEC));
    if (Clock.getDate() < 10)
        valuee_date = "s" + valuee_date;
    String valuee_month = decode_date(String(Clock.getMonth(Century), DEC));
    if (Clock.getMonth(Century) < 10)
        valuee_month = "s" + valuee_month;
    String valuee_year = decode_date(String(Clock.getYear(), DEC));
    if (Clock.getYear() < 10)
        valuee_year = "s" + valuee_year;
    valuee_year = "us" + valuee_year;
    String Time = "~" + valuee_year + " , " + valuee_month + " , " + valuee_date;
    return Time;
}

String get_Temp()
{
    int temperature = Clock.getTemperature();
    Serial.println(temperature);
    String temp = String(temperature);
    temp = decode_date(temp);
    if (temperature < 10)
        temp = "0" + temp;
    return temp;
}

String decode_date(String eng_date)
{
    uint8_t date_len = eng_date.length();
    String nep_date = "";
    for (uint8_t i = 0; i < date_len; i++)
    {
        if (eng_date[i] == '0')
        {
            nep_date = nep_date + "s";
        }
        else if (eng_date[i] == '1')
        {
            nep_date = nep_date + "t";
        }
        else if (eng_date[i] == '2')
        {
            nep_date = nep_date + "u";
        }
        else if (eng_date[i] == '3')
        {
            nep_date = nep_date + "v";
        }
        else if (eng_date[i] == '4')
        {
            nep_date = nep_date + "w";
        }
        else if (eng_date[i] == '5')
        {
            nep_date = nep_date + "x";
        }
        else if (eng_date[i] == '6')
        {
            nep_date = nep_date + "y";
        }
        else if (eng_date[i] == '7')
        {
            nep_date = nep_date + "z";
        }
        else if (eng_date[i] == '8')
        {
            nep_date = nep_date + "{";
        }
        else if (eng_date[i] == '9')
        {
            nep_date = nep_date + "|";
        }
    }
    return nep_date;
}

void mid_display_engfont(int y_coordinate, int x_old, String message, int view_window_size)
{
    int x_coordinate = (((view_window_size)-message.length() * 6) / 2) + x_old;
    dmd.drawString(x_coordinate, y_coordinate, message);
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

    nep.drawString(32, 16, get_date() + " . " + get_time(" - ") + " . ");
    delay_engine(500);
    nep.drawString(32, 16, get_date() + " . " + get_time("    ") + " . ");
    delay_engine(500);
}

void loop()
{
    motion.palika(0, 16);
    show_date_time();
    motion.ramechap1(31 + 14, 0);
}