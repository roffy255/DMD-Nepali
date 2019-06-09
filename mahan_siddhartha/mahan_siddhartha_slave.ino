#include <animation.h>
#include <EEPROM.h>
// #include <DS3231.h>
// #include <Wire.h>
#include <SPI.h>
#include <DMD2.h>
#include <DMD_Nepali.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/NepFont5x7.h>
#include <fonts/Arial_Black_16.h>

#define signal_out 5
#define signal_in 4


// DS3231 Clock;

bool setIPFlag = false;
bool configModeFlag = false;
bool ack = false;


bool h12;
bool PM;
bool Century = false;
bool time_state;
uint16_t offset_address = 0;
uint16_t actual_addr = 0;
String Message = "";

const uint8_t width = 5;
const uint8_t height = 1;
const uint8_t *FONTNep = NepFont5x7;
const uint8_t *FONTEng = SystemFont5x7;
const uint8_t *FONTBold = Arial_Black_16;
const String month_array[12] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
const String greeting_array[4] = {"Good Morning", "Good Afternoon", "Good Evening", "Good Night"};
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

SPIDMD dmd(width, height);
animation motion(dmd, dmd, width, height);


void setup()
{
    pinMode(signal_out, OUTPUT);
    pinMode(signal_in, INPUT);
    Serial.begin(9600);
    dmd.begin();
    EEPROM.begin(512);
    dmd.selectFont(FONTEng);
    dmd.setBrightness(255);
    digitalWrite(signal_out, LOW);
}



// void greeting()
// {
//     int timing = Clock.getHour(h12, PM);
//     Serial.println(timing);
//     if (timing >= 11 & timing < 17)
//     {
//         mid_display_engfont(4, 32, greeting_array[1], 128);
//         motion.ASUN_2(128, -8);
//         motion.ASUN_2(40, -8);
//     }
//     else if (timing >= 17 & timing < 20)
//     {
//         mid_display_engfont(4, 32, greeting_array[2], 128);
//         motion.ASUN_2(133, 0);
//         motion.ASUN_2(38, 0);
//     }
//     else if ((timing >= 20 & timing <= 23) || (timing < 4))
//     {
//         mid_display_engfont(4, 32, greeting_array[3], 128);
//         motion.moon(128, 0);
//         motion.moon(40, 0);
//     }
//     else if (timing >= 4 & timing < 11)
//     {
//         mid_display_engfont(4, 32, greeting_array[0], 128);
//         motion.sun_2(133, 0);
//         motion.sun_2(38, 0);
//     }
// }

void delay_engine(int delay_val)
{
    unsigned long startMs = millis();
    while (millis() - startMs <= delay_val)
    {
        set_brightness();
        delay(1);
    }
}

void set_brightness()
{
    dmd.setBrightness(EEPROM.read(510));
}

// String get_time(String seperator)
// {
//     String Time = "";
//     int hr = (Clock.getHour(h12, PM) % 12) == 0 ? 12 : (Clock.getHour(h12, PM) % 12); //if else
//     String valuee_hour = String(hr, DEC);
//     if (hr < 10)
//         valuee_hour = "0" + valuee_hour;
//     String valuee_minute = String(Clock.getMinute(), DEC);
//     if (Clock.getMinute() < 10)
//         valuee_minute = "0" + valuee_minute;
//     String valuee_second = String(Clock.getSecond(), DEC);
//     if (Clock.getSecond() < 10)
//         valuee_second = "0" + valuee_second;
//     Time = valuee_hour + seperator + valuee_minute;
//     if (Clock.getHour(h12, PM) >= 12)
//     {
//         time_state = true;
//     }
//     else
//     {
//         time_state = false;
//     }

//     Serial.println(Clock.getHour(h12, PM), DEC);
//     return Time;
// }

// String get_date(String seperator)
// {
//     String valuee_date = String(Clock.getDate(), DEC);
//     if (Clock.getDate() < 10)
//         valuee_date = "0" + valuee_date;
//     String valuee_month = month_array[Clock.getMonth(Century) - 1];
//     String valuee_year = String(Clock.getYear(), DEC);
//     if (Clock.getYear() < 10)
//         valuee_year = "0" + valuee_year;
//     valuee_year = "20" + valuee_year;
//     String Time = valuee_month + valuee_date;
//     return Time;
// }

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

// void WriteDateTimeIntoRTC(String input)
// {
//     uint16_t minuteVal, hr, sec;
//     uint16_t date, mth, yr;
//     uint16_t startindex = input.indexOf("->");
//     uint16_t endindex = input.indexOf("|");

//     date = (input.substring(startindex + 2, startindex + 4)).toInt();
//     mth = (input.substring(startindex + 5, startindex + 7)).toInt();
//     yr = (input.substring(startindex + 8, startindex + 10)).toInt();
//     hr = (input.substring(startindex + 11, startindex + 13)).toInt();
//     minuteVal = (input.substring(startindex + 14, startindex + 16)).toInt();
//     sec = (input.substring(startindex + 17, startindex + 19)).toInt();

//     Serial.println(date);
//     Serial.println(mth);
//     Serial.println(yr);
//     Serial.println(hr);
//     Serial.println(minuteVal);
//     Serial.println(sec);

//     Clock.setHour(hr);
//     Clock.setMinute(minuteVal);
//     Clock.setSecond(sec);
//     Clock.setYear(yr);
//     Clock.setMonth(mth);
//     Clock.setDate(date);
// }



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
    int trigger_right = 0;
    int trigger_left = width * 32;
    eeprom_read();
    uint16_t string_length = Message.length();
    for (int i = width * 32; i > -6; i--)
    {
        dmd.drawString(i, 4, Message);
        motion.snakeRight(0, trigger_right);
        trigger_right++;
        if (trigger_right > width * 32)
        {
            trigger_right = 0;
        }

        motion.snakeLeft(14, trigger_left);
        trigger_left--;
        if (trigger_left < 0)
        {
            trigger_left = width * 32;
        }
        delay_engine(40);
    }
    Message = Message.substring(1, string_length);
    int loop_length = string_length;
    for (int i = 0; i < loop_length; i++)
    {
        string_length = Message.length();

        for (int j = 0; j > -6; j--)
        {
            dmd.drawString(j, 4, Message);
            motion.snakeRight(0, trigger_right);
            trigger_right++;
            if (trigger_right > width * 32)
            {
                trigger_right = 0;
            }

            motion.snakeLeft(14, trigger_left);
            trigger_left--;
            if (trigger_left < 0)
            {
                trigger_left = width * 32;
            }
            delay_engine(40);
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
    for (int i = 0; i < loop_length; i++)
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

void displayBold(String msg)
{
    int trigger_right = 0;
    int trigger_left = width * 32;
    dmd.selectFont(Arial_Black_16);
    uint16_t string_length = msg.length();
    char buffer_char = msg[0];
    int char_ascii_num = int(buffer_char);
    int over_scroll_length = bold_array[char_ascii_num - 32];

    while(digitalRead(signal_in) != true){
        Serial.println("sleeping");
    }

    for (int i = width * 32; i > -(over_scroll_length + 1); i--)
    {
        dmd.drawString(i, 1, msg);
        delay_engine(30);  
    }
    msg = msg.substring(1, string_length);
    int loop_length = string_length;
    for (int i = 0; i < loop_length; i++)
    {
        string_length = msg.length();
        buffer_char = msg[0];
        char_ascii_num = int(buffer_char);
        over_scroll_length = bold_array[char_ascii_num - 32];

        for (int j = 0; j > -(over_scroll_length + 1); j--)
        {
            dmd.drawString(j, 1, msg);
            delay_engine(30);
        }
        msg = msg.substring(1, string_length);
        if (msg.length() == 0)
        {
            break;
        }
    }

    dmd.selectFont(FONTEng);

}

void loop()
{
    displayBold("NEW SUNSHINE SECONDARY SCHOOL");
    // dmd.selectFont(FONTEng);
    // //    dmd.drawString(0, 4, "NEW SUNSHINE SEC. SCHOOL");
    // mid_display_engfont(4, 0, "NEW SUNSHINE SEC. SCHOOL", 160);
    // delay_engine(5000);
    // motion.cleaningCurtain();
    // for (int i = 0; i <= 5; i++)
    // {
    //     greeting();
    //     show_date_time();
    // }
    // motion.cleaningCurtain();
    // display_message();
    // motion.cleaningCurtain();
}
