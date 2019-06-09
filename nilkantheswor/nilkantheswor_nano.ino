#include <animation.h>
#include <Wire.h>
#include <SPI.h>
#include <DMD2.h>
#include <fonts/Arial_Black_16.h>
#include <DMD_Nepali.h>

const uint8_t width = 5;
const uint8_t height = 1;
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

SoftDMD dmd(width, height);
animation motion(dmd, dmd, width, height);
DMD_Nepali nep(dmd);
uint8_t speed = 1;
void setup()
{
    Wire.begin();
    Serial.begin(9600);
    dmd.begin();
    dmd.selectFont(Arial_Black_16);
    dmd.setBrightness(255);
    dmd.drawString(32, 0, "Loading . . .");
    delay(7000);
}

void displayBold(String Message)
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
        delay(speed);
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
            delay(speed);
        }
        Message = Message.substring(1, string_length);
        if (Message.length() == 0)
        {
            break;
        }
    }
}


void loop()
{
    dmd.clearScreen();
    nep.drawString(14,0,"~H`R5Cm@gVmUP     %^OR     Xm?kP");
    delay(5000);
    dmd.clearScreen();
    displayBold("NILKANTHESWOR OIL STORE");
    dmd.clearScreen();
}
