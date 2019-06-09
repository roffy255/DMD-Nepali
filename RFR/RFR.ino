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
const char *ssidAP = "room4reading";
const char *passwordAP = "room4reading";
const char *ota_hostname = "rfrota";
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

const uint8_t width = 3;
const uint8_t height = 2;
const uint8_t *FONTNep = NepFont5x7;
const uint8_t *FONTEng = SystemFont5x7;
const uint8_t *FONTBold = Arial_Black_16;
const String month_array[12] = {"JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE", "JULY", "AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER"};
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
        server.on("/7555368cc97a9bc6ac2f4bafb1d51c5c", handleQueryArgs);
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
    WiFi.begin("MSF755368", "MSF755368");
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
        EEPROM.write(511, 60 / (value.toInt() + 1));
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
    String valuee_hour = String(hr, DEC);
    if (hr < 10)
    {
        valuee_hour = "0" + valuee_hour;
    }
    String valuee_minute = String(Clock.getMinute(), DEC);
    if (Clock.getMinute() < 10)
        valuee_minute = "0" + valuee_minute;
    String valuee_second = String(Clock.getSecond(), DEC);
    if (Clock.getSecond() < 10)
        valuee_second = "0" + valuee_second;

    if (Clock.getHour(h12, PM) >= 12)
    {
        //        time_state = true;
        Time = valuee_hour + seperator + valuee_minute + " PM " + get_Temp();
    }
    else
    {
        //        time_state = false;
        Time = valuee_hour + seperator + valuee_minute + " AM " + get_Temp();
    }

    Serial.println(Clock.getHour(h12, PM), DEC);
    Serial.println(get_Temp());
    return Time;
}

String get_date()
{
    String valuee_date = String(Clock.getDate(), DEC);
    if (Clock.getDate() < 10)
        valuee_date = "0" + valuee_date;
    String valuee_month = month_array[Clock.getMonth(Century) - 1];
    String valuee_year = String(Clock.getYear(), DEC);
    if (Clock.getYear() < 10)
        valuee_year = "0" + valuee_year;
    valuee_year = "20" + valuee_year;
    String Time = valuee_year + " " + valuee_month + " " + valuee_date;
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
    mid_display_engfont(6, 0, get_date(), 96);
    dmd.drawString(7-3, 17, get_time(":"));
    delay_engine(500);
    mid_display_engfont(6, 0, get_date(), 96);
    dmd.drawString(7-3, 17, get_time(" "));
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
    uint16_t string_length = Message.length();
    char buffer_char = Message[0];
    int char_ascii_num = int(buffer_char);
    int over_scroll_length = bold_array[char_ascii_num - 32];
    for (int i = width * 32; i > -(over_scroll_length + 1); i--)
    {
        dmd.drawString(i, 7, Message);
        motion.snakeRight(0, trigger_right);
        trigger_right++;
        if (trigger_right > width * 32)
        {
            trigger_right = 0;
        }

        motion.snakeLeft(30, trigger_left);
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
        buffer_char = Message[0];
        char_ascii_num = int(buffer_char);
        over_scroll_length = bold_array[char_ascii_num - 32];

        for (int j = 0; j > -(over_scroll_length + 1); j--)
        {
            dmd.drawString(j, 7, Message);
            motion.snakeRight(0, trigger_right);
            trigger_right++;
            if (trigger_right > width * 32)
            {
                trigger_right = 0;
            }

            motion.snakeLeft(30, trigger_left);
            trigger_left--;
            if (trigger_left < 0)
            {
                trigger_left = width * 32;
            }
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

void loop()
{
    if (OTA_flag)
    {
        ArduinoOTA.handle();
    }
    else
    {
        nep.drawString(22, 0, "~JBmHg  5k@^");
        dmd.selectFont(FONTEng);
        dmd.drawString(0, 18, "Room For Reading");
        delay_engine(5000);
        motion.cleaningCurtain();
        dmd.selectFont(FONTEng);

        for (int i = 0; i <= 5; i++)
        {
            dmd.setPixel(87-3, 16, GRAPHICS_ON);
            dmd.setPixel(88-3, 16, GRAPHICS_ON);
            dmd.setPixel(87-3, 17, GRAPHICS_ON);
            dmd.setPixel(88-3, 17, GRAPHICS_ON);
            dmd.drawString(90-3, 17, "C");
            show_date_time();
            show_date_time();
        }
        motion.cleaningCurtain();
        dmd.selectFont(FONTEng);
        displayBold();
        motion.cleaningCurtain();
    }
}
