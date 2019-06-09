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

String Message = "";

const uint8_t width = 5;
const uint8_t height = 2;

SPIDMD dmd(width, height);
DMD_Nepali nep(dmd);

void setup()
{
    Wire.begin();
    Serial.begin(9600);
    dmd.begin();
    EEPROM.begin(512);
    dmd.setBrightness(255);
}

void displayBoldNepali(int y)
{

    Message = "OY^!YPbR^( P^Ng;^J H7PJ^R_5^N^ XmU^7D ; q";
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
        delay(30);
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

        if (((*next == '@' || *next == 'A' || *next == 'B' || *next == '?' || *next == ';' || *next == '9' || *next == 'Y') && (*(next + 1) == 'm')) || ((*(next + 1) == 'm') && *(next + 2) == ' ')) //----------> for khutta kateko tah, ta, tha, cha, nga etc and last letter ko khutta kateko
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
            Serial.println(Message);
            delay(30);
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

void loop()
{
    displayBoldNepali(8);
    dmd.clearScreen();
}