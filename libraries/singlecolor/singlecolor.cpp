#include <singlecolor.h>

singlecolor::singlecolor(DMDFrame &dmd, SPIDMD &dmdbright, ESP8266WebServer &serve, animation &motion, int width, int height) : dmd(dmd),
                                                                                    dmdbright(dmdbright),
                                                                                    serve(serve),
                                                                                    motion(motion),
                                                                                    width(width),
                                                                                    height(height)
{
    begin();
}

void singlecolor::begin(){
    Serial.begin(9600);
    EEPROM.begin(512);

}

void singlecolor::displayBold(int display_coordinate, int boarder_coordinate_header, int boarder_coordinate_footer)
{
}

void singlecolor::displayNepali(int display_coordinate, int boarder_coordinate_header, int boarder_coordinate_footer)
{
}
void singlecolor::displayNormal(int display_coordinate, int boarder_coordinate_header, int boarder_coordinate_footer)
{
    int count = 0;
    int trigger_right = 0;
    int trigger_left = width * 32;
    eeprom_read();
    uint16_t string_length = Message.length();
    for (int i = width * 32; i > -6; i--)
    {
        dmd.drawString(i, 20, Message);
        motion.snakeRight(boarder_coordinate_header, trigger_right);
        trigger_right++;
        if (trigger_right > width * 32)
        {
            trigger_right = 0;
        }

        motion.snakeLeft(boarder_coordinate_footer, trigger_left);
        trigger_left--;
        if (trigger_left < 0)
        {
            trigger_left = width * 32;
        }
        delay(20);
    }
    Message = Message.substring(1, string_length);
    int loop_length = string_length;
    for (int i = 0; i < loop_length; i++)
    {
        string_length = Message.length();

        for (int j = 0; j > -6; j--)
        {
            dmd.drawString(j, 20, Message);
            motion.snakeRight(16, trigger_right);
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
            delay(20);
        }

        Message = Message.substring(1, string_length);
    }
}

void singlecolor::eeprom_read()
{
    Message = "";
    int x = EEPROM.read(0) + EEPROM.read(1);
    for (int i = 0; i < (x); i++)
    {
        char bufferr = EEPROM.read(i + 2);
        Message = String(Message + bufferr);
        Serial.println(Message);
    }
}
