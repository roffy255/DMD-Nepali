#include <animation.h>
#include <EEPROM.h>
#include <Wire.h>
#include <SPI.h>
#include <DMD2.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/NepFont5x7.h>
#include <fonts/Arial_Black_16.h>
#include <DMD_Nepali.h>

bool setIPFlag = false;
bool configModeFlag = false;
bool ack = false;

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
DMD_Nepali nep(dmd);

void setup()
{
    Wire.begin();
    Serial.begin(9600);
    dmd.begin();
    EEPROM.begin(512);
}

void delay_engine(int delay_val)
{
    unsigned long startMs = millis();
    while (millis() - startMs <= delay_val)
    {

        CheckSerialInput();
        set_brightness();
        speed_control();
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
    if (sub == "M")
    {
        DecodeAndSaveMessage(input);
    }
    else if (sub == "B")
    {
        DecodeAndSaveBrightness(input);
    }
    else if (sub == "S")
    {
        DecodeAndSaveSpeed(input);
    }
}

void DecodeAndSaveMessage(String msg)
{
    Message = msg.substring(msg.indexOf("->") + 2, msg.indexOf("|"));
    write_to_EEPROM(0);
}

void DecodeAndSaveBrightness(String In_val)
{
    uint16_t startindex = In_val.indexOf("->");
    uint16_t endindex = In_val.indexOf("|");
    int screen_brightness = In_val.substring(startindex + 2, endindex).toInt();
    EEPROM.write(510, screen_brightness);
    EEPROM.commit();
}

void DecodeAndSaveSpeed(String In_val)
{
    uint16_t startindex = In_val.indexOf("->");
    uint16_t endindex = In_val.indexOf("|");
    int screen_speed = In_val.substring(startindex + 2, endindex).toInt();
    EEPROM.write(508, screen_speed);
    EEPROM.commit();
}

void set_brightness()
{
    dmd.setBrightness(EEPROM.read(510));
}

void speed_control()
{
    speed = EEPROM.read(508);
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
        dmd.drawString(i, 9, Message);
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
            dmd.drawString(j, 9, Message);
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

void displayBoldNepali(int y)
{
    eeprom_read();
    Message = Message.substring(1, Message.length()) + " ";
    int length = Message.length() + 1;
    char *m = new char[length];
    Message.toCharArray(m, length);
    char *next = &m[0];
    uint8_t cutter = 0;
    uint8_t old_cutter = 0;
    uint8_t kota_cutter = 0;
    uint8_t extender = 0;
    length--;

    //**************************first stage*************************************//

    for (int i = width * 32; i > 0; i--)
    {
        nep.drawBoldString(i, y, Message);
        delay_engine(speed);
    }

    while (length > 0)
    {
        kota_cutter = 0;
        if (((*next == '5' && *(next + 2) == 'W') || (*next == 'D' && *(next + 2) == 'P') || (*next == '<' && *(next + 2) == '>')) && *(next + 1) == 'm')
        { // if kshya, tra and gya
            if (*next == '5')
            {
                *(next + 2) = 'Z';
            }
            else if (*next == 'D')
            {
                *(next + 2) = '[';
            }
            else if (*next == '<')
            {
                *(next + 2) = 'r';
            }
            length = length - 2;
            kota_cutter = kota_cutter + 2;
            cutter = cutter + kota_cutter;
            next = &m[cutter];
            kota_cutter = 0;
        }

        if (*next == 'F' && *(next + 1) == 'm' && *(next + 2) == 'G')
        {
            *(next + 2) = 'n';
            length = length - 2;
            kota_cutter = kota_cutter + 2;
            cutter = cutter + kota_cutter;
            next = &m[cutter];
            kota_cutter = 0;
        }

        if (*next == 'P' && *(next + 1) == 'b')
        {
            *(next + 1) = '1';
            length--;
            kota_cutter = kota_cutter + 1;
            cutter = cutter + kota_cutter;
            next = &m[cutter];
            kota_cutter = 0;
        }

        if (*next == '(')
        {
            extender += dmd.charWidth('\'');
            length--;
            kota_cutter++;
            goto terminal;
        }

        if (*next == '0')
        {
            extender += dmd.charWidth('/');
            length--;
            kota_cutter++;
            goto terminal;
        }

        if (*(next + 2) == 'P' && (*next == '5' || *next == 'L' || *next == 'M' || *next == 'J') && *(next + 1) == 'm')
        { // if followed by Ra
            if (*next == '5')
            {
                *(next + 2) = 'S';
                extender += dmd.charWidth('S');
            }
            else if (*next == 'J')
            {
                *(next + 2) = 'T';
                extender += dmd.charWidth('T');
            }
            else if (*next == 'L')
            {
                *(next + 2) = 'Q';
                extender += dmd.charWidth('Q');
            }
            else if (*next == 'M')
            {
                *(next + 2) = 'I';
                extender += dmd.charWidth('I');
            }
            length = length - 2;
            kota_cutter = kota_cutter + 2;
            cutter = cutter + kota_cutter;
            next = &m[cutter];
            kota_cutter = 0;
        }

        if (*next == 'P' && *(next + 1) == 'm')
        {
            if (*(next + 3) == '_')
            {

                extender += dmd.charWidth('^');
                length--;
                kota_cutter = kota_cutter + 1;
            }

            if (*(next + 3) == 'g' || *(next + 3) == 'h')
            {
                length--;
                kota_cutter = kota_cutter + 1;
            }

            if (*(next + 3) == 'a' || *(next + 3) == 'b')
            {
                length--;
                kota_cutter = kota_cutter + 1;
            }

            length--;
            kota_cutter++;

            if (*(next + 3) == '`')
            {
                extender += dmd.charWidth('^');
                length--;
                kota_cutter = kota_cutter + 1;
            }

            if (*(next + 3) == 'k' || *(next + 3) == 'l')
            {
                extender += dmd.charWidth('^');
                length--;
                kota_cutter = kota_cutter + 1;
            }

            if (*(next + 3) == '^')
            {
                extender += dmd.charWidth('^');
                length--;
                kota_cutter = kota_cutter + 1;
            }

            length = length - 2;
            kota_cutter = kota_cutter + 2;
            extender += dmd.charWidth(*(next + 2));

            goto terminal;
        }

        if (((*next == '@' || *next == 'A' || *next == 'B' || *next == '?' || *next == ';' || *next == '9' || *next == 'Y') && (*(next + 1) == 'm')) || ((*(next + 1) == 'm') && (*(next + 2) == ' '))) //----------> for khutta kateko tah, ta, tha, cha, nga etc and last letter ko khutta kateko

        {
            extender += dmd.charWidth(*next);
            length = length - 2;
            kota_cutter = kota_cutter + 2;
            goto terminal;
        }
        else if (*(next + 1) == 'm' && *(next + 2) != ' ') //-----------------> khutta kateko (for the middle alphabet (or not the last one))(adhi na, adhi ka etc);
        {
            int8_t calliber = 0;
            if (*(next + 3) == '_')
            {
                extender += dmd.charWidth('^');
                length--;
                kota_cutter = kota_cutter + 1;
            }

            extender += dmd.charWidth(*next);
            if (*next == '6' || *next == '7' || *next == '8' || *next == ':' || *next == '<' || *next == '=' || *next == 'C' || *next == 'J' || *next == 'L' || *next == 'M' || *next == 'N' || *next == 'D' || *next == 'E' || *next == 'G' || *next == 'H' || *next == 'O' || *next == 'R' || *next == 'U' || *next == 'V' || *next == 'W' || *next == 'X' || *next == '>' || *next == '%')
                calliber = -6;

            if (*next == '5' || *next == 'K')
                calliber = -((dmd.charWidth(*next) / 3));

            extender += extender + calliber;

            if (*(next + 3) == 'a' || *(next + 3) == 'b')
            {
                length--;
                kota_cutter = kota_cutter + 1;
            }

            if (*(next + 3) == '!' || *(next + 3) == '\"')
            {

                length--;
                kota_cutter = kota_cutter + 1;
            }

            if (*(next + 3) == 'g' || *(next + 3) == 'h')
            {
                length--;
                kota_cutter = kota_cutter + 1;
            }

            if (*(next + 3) == '`')
            {
                extender += dmd.charWidth('^');
                length--;
                kota_cutter = kota_cutter + 1;
            }

            if (*(next + 1) == 'k' || *(next + 1) == 'l')
            {

                extender += dmd.charWidth('^');
                length--;
                kota_cutter = kota_cutter + 1;
            }
            length = length - 3;
            kota_cutter = kota_cutter + 3;
            goto terminal;
        }

        if (*(next + 1) == '!' || *(next + 1) == '/"' || *(next + 1) == '^' || *(next + 1) == 'g' || *(next + 1) == 'h' || *(next + 1) == 'a' || *(next + 1) == 'b' || *(next + 1) == 'c' || *(next + 1) == 'k' || *(next + 1) == 'l' || *(next + 1) == '_' || *(next + 1) == '`' || *(next + 1) == '^')
        {

            if (*(next + 1) == '_')
            {
                extender += dmd.charWidth('^');
                length--;
                kota_cutter = kota_cutter + 1;
            }

            extender += dmd.charWidth(*next);

            if (*(next + 1) == '!' || *(next + 1) == '\"')
            {

                length--;
                kota_cutter = kota_cutter + 1;
            }

            if (*(next + 1) == 'g' || *(next + 1) == 'h')
            {

                length--;
                kota_cutter = kota_cutter + 1;
            }

            if (*(next + 1) == 'c')
            {
                length--;
                kota_cutter++;
            }

            if (*(next + 1) == 'a' || *(next + 1) == 'b')
            {
                length--;
                kota_cutter = kota_cutter + 1;
            }

            if (*(next + 1) == '`')
            {

                extender += dmd.charWidth('^');
                length--;
                kota_cutter = kota_cutter + 1;
            }

            if (*(next + 1) == 'k' || *(next + 1) == 'l')
            {
                extender += dmd.charWidth('^');
                length--;
                kota_cutter = kota_cutter + 1;
            }

            if (*(next + 1) == '^')
            {

                extender += dmd.charWidth('^');
                length--;
                kota_cutter = kota_cutter + 1;
                if (*(next + 2) == '!')
                {
                    length--;
                    kota_cutter++;
                }
            }

            length--;
            kota_cutter = kota_cutter + 1;
        }
        else
        {

            extender += dmd.charWidth(*next);
            length--;
            kota_cutter = kota_cutter + 1;
        }

        goto terminal;
    terminal:
        Serial.println(extender);
        for (int i = 0; i > -extender; i--)
        {
            nep.drawBoldString(i, y, Message);
            delay_engine(speed);
        }
        extender = 0;
        cutter = cutter + kota_cutter;
        next = &m[cutter];
        if (Message.length() <= 0)
        {
            break;
        }
        Message = Message.substring(cutter - old_cutter, Message.length());
        old_cutter = cutter;
    }
}
void display_msg()
{
    eeprom_read();
    Serial.println(Message);
    if (Message[0] == '~')
    {
        displayBoldNepali(8);
    }
    if (Message[0] == '}')
    {
        displayBold();
    }
}

void loop()
{
    display_msg();
    dmd.clearScreen();
    delay_engine(100);
}