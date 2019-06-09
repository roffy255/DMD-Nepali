#include <animation.h>
#include <EEPROM.h>
#include <DS3231.h>
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
#include <ArduinoOTA.h>

DS3231 Clock;

bool setIPFlag = false;
bool configModeFlag = false;
bool ack = false;

//Wifi
//const String startConfigMsg = "sc";
const char *ssidAP = "Hotelhalesi";
const char *passwordAP = "HOTELHALESI123";
const char *ota_hostname = "halesiota";
IPAddress apIP(192, 168, 4, 1);

ESP8266WebServer server(80);

bool h12;
bool PM;
bool Century = false;
bool time_state;
uint16_t offset_address = 0;
uint16_t message_count = 0;
uint16_t actual_addr = 0;
String Message = "";
bool OTA_flag = false;
uint8_t speed;

const uint8_t width = 5;
const uint8_t height = 1;
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
    connectToAP();

    if (!OTA_flag)
    {

        beginAP(ssidAP, passwordAP, apIP);
        delay(500);
        server.on("/", handleRoot);
        server.on("/0cadbf4ad91fff00209192332706e37d", handleQueryArgs);
        server.onNotFound(handleNotFound);
        server.begin();
        delay(500);
        dmd.begin();
        EEPROM.begin(512);
        dmd.selectFont(FONTEng);
        dmd.setBrightness(255);
        speed_control();
    }
    else
    {
        delay(500);
        beginOTA(ota_hostname);
        delay(500);
    }
}

// starts an access point
void beginAP(const char *ssid, const char *password, IPAddress ip)
{
    //WiFi.mode(WIFI_AP);
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(ip, ip, IPAddress(255, 255, 255, 0));
    WiFi.softAP(ssidAP, passwordAP);
    Serial.print("AP is up at ");
    Serial.println(WiFi.softAPIP());
}

// connects to an access point
void connectToAP()
{
    Serial.print("");
    WiFi.begin("MSF75536", "MSF75536");
    int timer1 = millis();
    OTA_flag = true;
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
        if (millis() - timer1 > 7000)
        {
            Serial.println("connection failed");
            OTA_flag = false;
            break;
        }
    }
    Serial.print("\nConnected to '");
    Serial.print(ssidAP);
    Serial.print("' and got IP ");
    Serial.println(WiFi.localIP());
}

// starts OTA server
void beginOTA(const char *hostname)
{
    ArduinoOTA.setHostname(hostname);
    ArduinoOTA.onStart([]() {
        Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
            Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
            Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
            Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
            Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
            Serial.println("End Failed");
    });
    ArduinoOTA.begin();
    Serial.print("OTA is up");
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
        Serial.println(Message);
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
    else if (key == "S")
    {
        EEPROM.write(511, value.toInt());
        EEPROM.commit();
        speed_control();
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

void speed_control()
{
    speed = EEPROM.read(511);
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

// void show_date_time()
// {

//     nep.drawString(0, 0, get_date() + "   " + get_time(":") + "       " + "D^J  " + get_Temp());
//     motion.dateSeperator(28, 0);
//     motion.dateSeperator(46, 0);
//     motion.divider(66, 0);
//     motion.divider(105, 0);
//     motion.timeSeperator(84, 0);
//     motion.degreeC(150, 3);
//     delay_engine(500);
//     nep.drawString(0, 0, get_date() + "   " + get_time(":") + "       " + "D^J  " + get_Temp());
//     motion.dateSeperator(28, 0);
//     motion.dateSeperator(46, 0);
//     motion.divider(66, 0);
//     motion.divider(105, 0);
//     motion.timeSeperatorCompliment(84, 0);
//     motion.degreeC(150, 3);
//     delay_engine(500);
// }

void show_date_time()
{
    nep.drawString(0, 0, ". " + get_date() + " . " + get_time(" - ") + " . " + "D^J  " + get_Temp() + "$ .");
    delay_engine(500);
    nep.drawString(0, 0, ". " + get_date() + " . " + get_time("    ") + " . " + "D^J  " + get_Temp() + "$ .");
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
        dmd.drawString(i, 8, Message);
        motion.snakeRight(4, trigger_right);
        trigger_right++;
        if (trigger_right > width * 32)
        {
            trigger_right = 0;
        }

        motion.snakeLeft(18, trigger_left);
        trigger_left--;
        if (trigger_left < 0)
        {
            trigger_left = width * 32;
        }
        delay_engine(speed);
    }
    Message = Message.substring(1, string_length);
    int loop_length = string_length;
    for (int i = 0; i < loop_length; i++)
    {
        string_length = Message.length();

        for (int j = 0; j > -6; j--)
        {
            dmd.drawString(j, 8, Message);
            motion.snakeRight(4, trigger_right);
            trigger_right++;
            if (trigger_right > width * 32)
            {
                trigger_right = 0;
            }

            motion.snakeLeft(18, trigger_left);
            trigger_left--;
            if (trigger_left < 0)
            {
                trigger_left = width * 32;
            }
            delay_engine(speed);
        }
        Message = Message.substring(1, string_length);
    }

    actual_addr = 0;
}

void displayBold()
{
    int count = 0;
    int trigger_right = 0;
    int trigger_left = width * 32;
    dmd.selectFont(Arial_Black_16);
    eeprom_read();
    Message = Message.substring(1, Message.length());
    uint16_t string_length = Message.length();
    char buffer_char = Message[0];
    int char_ascii_num = int(buffer_char);
    int over_scroll_length = bold_array[char_ascii_num - 32];
    for (int i = width * 32; i > -(over_scroll_length + 1); i--)
    {
        dmd.drawString(i, 0, Message);
        delay_engine(speed);
        Serial.println(Message);
        Serial.println(char_ascii_num);
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
            dmd.drawString(j, 0, Message);
            delay_engine(speed);
        }
        Message = Message.substring(1, string_length);
        Serial.println(Message);
        Serial.println(char_ascii_num);
        if (Message.length() == 0)
        {
            break;
        }
    }
    dmd.selectFont(FONTEng);
}

void displayNepali()
{
    eeprom_read();
    Message = Message + " ";
    uint16_t MessageLength = Message.length();
    Message = Message.substring(1, MessageLength);
    char buffer_char = Message[0];
    uint8_t char_ascii_num = int(buffer_char);
    uint16_t over_scroll_length;
    uint8_t cutter;

    /***********************first stage checking**************************/
    if (Message[1] == 'c' || Message[1] == 'g' || Message[1] == 'h' || Message[1] == 'a' || Message[1] == 'b' || Message[1] == '"' || Message[1] == '!')
    {
        over_scroll_length = nep_array[char_ascii_num - 32];
        cutter = 2;
    }
    else if (Message[1] == 'l' || Message[1] == '_' || Message[1] == 'k' || Message[1] == '^' || Message[1] == '`')
    {
        over_scroll_length = nep_array[char_ascii_num - 32] + 2;
        cutter = 2;
    }
    else if (Message[1] == 'm' & (Message[0] != '}' & Message[0] != '5' & Message[0] != '[' & Message[0] != 'B' & Message[0] != '?' & Message[0] != '@') & Message.length() > 2)
    {
        over_scroll_length = nep_array[int(Message[2]) - 32] + (nep_array[char_ascii_num - 32] - 2);
        cutter = 3;
        if (Message[3] == 'l' || Message[3] == '_' || Message[3] == 'k' || Message[3] == '^' || Message[3] == '`')
        {
            over_scroll_length = nep_array[int(Message[2]) - 32] + (nep_array[char_ascii_num - 32] - 2) + 2;
            cutter = 4;
        }
        else if (Message[3] == 'c' || Message[3] == 'g' || Message[3] == 'h' || Message[3] == 'a' || Message[3] == 'b' || Message[3] == '"' || Message[3] == '!')
        {
            over_scroll_length = nep_array[int(Message[2]) - 32] + (nep_array[char_ascii_num - 32] - 2);
            cutter = 4;
        }
    }
    else if (Message[1] == 'm' & (Message[0] == '}' || Message[0] == '5' || Message[0] == '[' || Message[0] == 'B' || Message[0] == '?' || Message[0] == '@'))
    {
        over_scroll_length = nep_array[char_ascii_num - 32];
        cutter = 2;
        Serial.println("i am on");
    }
    else if (Message[1] == 'm' & (Message[0] != '}' & Message[0] != '5' & Message[0] != '[' & Message[0] != 'B' & Message[0] != '?' & Message[0] != '@') & Message.length() <= 2)
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
        nep.drawString(i, 1, "~" + Message);
        delay_engine(speed);
    }

    /******************************second stage*********************/

    Message = Message.substring(cutter, Message.length());
    int loop_length = Message.length();
    for (int i = 0; i < loop_length;)
    {
        buffer_char = Message[0];
        char_ascii_num = int(buffer_char);

        if (Message[1] == 'c' || Message[1] == 'g' || Message[1] == 'h' || Message[1] == 'a' || Message[1] == 'b' || Message[1] == '"' || Message[1] == '!')
        {
            over_scroll_length = nep_array[char_ascii_num - 32];
            cutter = 2;
        }
        else if (Message[1] == 'l' || Message[1] == '_' || Message[1] == 'k' || Message[1] == '^' || Message[1] == '`')
        {
            over_scroll_length = nep_array[char_ascii_num - 32] + 2;
            cutter = 2;
        }
        else if (Message[1] == 'm' & (Message[0] != '}' & Message[0] != '5' & Message[0] != '[' & Message[0] != 'B' & Message[0] != '?' & Message[0] != '@') & Message.length() > 2)
        {
            over_scroll_length = nep_array[int(Message[2]) - 32] + (nep_array[char_ascii_num - 32] - 2);
            cutter = 3;
            if (Message[3] == 'l' || Message[3] == '_' || Message[3] == 'k' || Message[3] == '^' || Message[3] == '`')
            {
                over_scroll_length = nep_array[int(Message[2]) - 32] + (nep_array[char_ascii_num - 32] - 2) + 2;
                cutter = 4;
            }
            else if (Message[3] == 'c' || Message[3] == 'g' || Message[3] == 'h' || Message[3] == 'a' || Message[3] == 'b' || Message[3] == '"' || Message[3] == '!')
            {
                over_scroll_length = nep_array[int(Message[2]) - 32] + (nep_array[char_ascii_num - 32] - 2);
                cutter = 4;
            }
        }
        else if (Message[1] == 'm' & (Message[0] == '}' || Message[0] == '5' || Message[0] == '[' || Message[0] == 'B' || Message[0] == '?' || Message[0] == '@'))
        {
            over_scroll_length = nep_array[char_ascii_num - 32];
            cutter = 2;
            Serial.println("i am on");
        }
        else if (Message[1] == 'm' & (Message[0] != '}' & Message[0] != '5' & Message[0] != '[' & Message[0] != 'B' & Message[0] != '?' & Message[0] != '@') & Message.length() <= 2)
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
            nep.drawString(j, 1, "~" + Message);
            delay_engine(speed);
        }
        Message = Message.substring(cutter, Message.length());
        if (Message.length() == 0)
        {
            break;
        }
    }
    dmd.clearScreen();
    dmd.selectFont(SystemFont5x7);
}

void displayconstBold(String Message)
{
    int count = 0;
    int trigger_right = 0;
    int trigger_left = width * 32;
    dmd.selectFont(Arial_Black_16);
    uint16_t string_length = Message.length();
    char buffer_char = Message[0];
    int char_ascii_num = int(buffer_char);
    int over_scroll_length = bold_array[char_ascii_num - 32];
    for (int i = width * 32; i > -(over_scroll_length + 1); i--)
    {
        dmd.drawString(i, 1, Message);
        delay_engine(speed);
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
            dmd.drawString(j, 1, Message);
            delay_engine(speed);
        }
        Message = Message.substring(1, string_length);
       
        if (Message.length() == 0)
        {
            break;
        }
    }
    dmd.selectFont(FONTEng);
}

void display_msg()
{
    eeprom_read();
    if (Message[0] == '~')
    {
        displayNepali();
        dmd.selectFont(FONTEng);
    }
    if (Message[0] == '}')
    {
        displayBold();
    }
}

void loop()
{
    if (OTA_flag)
    {
        ArduinoOTA.handle();
    }
    else
    {
        //// nep.drawString(32, 0, "~Yk?R  XDm5^P  YRgX`");
        motion.hotelhalesi(32, 0);
        delay_engine(3000);
        dmd.clearScreen();
        dmd.selectFont(FONTEng);
        displayconstBold("HOTEL SATKAR, HALESI");
        for (int i = 0; i < 5; i++)
        {
            show_date_time();
            //      dmd.clearScreen();
        }
        motion.cleaningCurtain();
        display_msg();
        dmd.clearScreen();
    }
}
