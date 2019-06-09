#include "DMD_Nepali.h"

DMD_Nepali::DMD_Nepali(DMDFrame &dmd, int left, int top, int width, int height) : dmd(dmd),
                                                                                  inverted(false),
                                                                                  left(left),
                                                                                  top(top),
                                                                                  width(width),
                                                                                  height(height),
                                                                                  cur_x(0),
                                                                                  cur_y(0),
                                                                                  pending_newline(false)
{
}

//******************************************************************************************************************

void DMD_Nepali::checkScroll()
{
  while ((cur_x > 0 && cur_x + char_width >= width) || pending_newline)
  { // Need to wrap to new line
    if (pending_newline)
    { // No room, so just clear display
      clear();
    }
    else
    { // Scroll characters horizontally
      scroll_flag = true;
      scroll_by = char_width - (width - cur_x - 1);
      scrollX(-scroll_by);
    }
    pending_newline = false;
  }
}

//******************************************************************************************************************

uint8_t DMD_Nepali::getCharacterCount()
{
  if (char_count == 17)
  {
  }
  // Serial.print("in header count = ");
  // Serial.println(char_count);
  return (char_count);
}

//******************************************************************************************************************

char *DMD_Nepali::charMappingESP(char *MESSAGE)
{
  uint8_t i = 0;
  uint8_t j = 0;
  char_count = 0;

  while ((int)MESSAGE[i] != 0)
  {
    if (MESSAGE[i] <= 126)
    { // if the character is english ascii value is positive
      if (i == 0 || MESSAGE[i - 1] > 126)
      {
        m[j] = '}';
        j++;
        char_count++;
      }
      m[j] = MESSAGE[i];
      j++;
      char_count++;
    }
    else if (MESSAGE[i] == 224)
    { // if character is nepali
      if (i == 0 || MESSAGE[i - 1] <= 126)
      {
        m[j] = '~';
        j++;
        char_count++;
      }
      int temp1 = (int)MESSAGE[i];
      int temp2 = (int)MESSAGE[i + 1];
      int temp3 = (int)MESSAGE[i + 2];
      if (temp1 == 224)
      {
        if (temp2 == 164)
        {
          m[j] = temp3 - 96;
          j++;
          char_count++;
        }
        else if (temp2 == 165)
        {
          if (temp3 <= 144)
          {
            m[j] = temp3 - 32;
          }
          else if (temp3 >= 164)
          {
            m[j] = temp3 - 51;
          }
          j++;
          char_count++;
        }
      }
    }
    i++;
  }
  return (&m[0]);
}

//******************************************************************************************************************

char *DMD_Nepali::charMapping(char *MESSAGE)
{
  uint8_t i = 0;
  uint8_t j = 0;
  char_count = 0;

  while ((int)MESSAGE[i] != 0)
  {
    if (MESSAGE[i] == ' ')
    { // if the character is space
      m[j] = ' ';
      j++;
      char_count++;
    }
    else if (MESSAGE[i] >= 0)
    { // if the character is english ascii value is positive
      if (i == 0 || MESSAGE[i - 1] < 0 || (MESSAGE[i - 1] == ' ' && MESSAGE[i - 2] < 0))
      {
        m[j] = '}';
        j++;
        char_count++;
      }
      m[j] = MESSAGE[i];
      j++;
      char_count++;
    }
    else
    { // if character is negative ascii starts from negative
      if (i == 0 || MESSAGE[i - 1] >= 0 || (MESSAGE[i - 1] == ' ' && MESSAGE[i - 2] >= 0))
      {
        m[j] = '~';
        j++;
        char_count++;
      }
      int temp1 = (int)MESSAGE[i];
      int temp2 = (int)MESSAGE[i + 1];
      int temp3 = (int)MESSAGE[i + 2];
      if (temp1 == -32)
      {
        if (temp2 == -92)
        {
          m[j] = temp3 + 160;
          j++;
          char_count++;
        }
        else if (temp2 == -91)
        {
          if (temp3 <= -112)
          {
            m[j] = temp3 + 224;
          }
          else if (temp3 >= -92)
          {
            m[j] = temp3 + 205;
          }
          j++;
          char_count++;
        }
      }
    }
    i++;
  }
  return (&m[0]);
}

//*******************************************************************************************************************

//******************************************************************************************************************

char *DMD_Nepali::charMappingFromInt(char *MESSAGE, char delimiter)
{
  char *parsedMsg = parseCsv(MESSAGE, delimiter);
  return charMapping(parsedMsg);
}

char *DMD_Nepali::parseCsv(char *MESSAGE, char delimiter)
{
  String tempMsg(MESSAGE);
  char result[450] = {0};
  int charLength = 0;
  while (tempMsg.indexOf(delimiter) != -1)
  {
    int endIndex = tempMsg.indexOf(delimiter);
    // Serial.println(tempMsg.substring(0,endIndex));
    result[charLength] = (char)tempMsg.substring(0, endIndex).toInt();
    tempMsg = tempMsg.substring(endIndex + 1);
    //  Serial.println(result[charLength]);
    charLength++;
    // Serial.println(tempMsg);
  }
  // for(int i = 0; i < charLength; i++)
  //   Serial.print(result[i]);
  return result;
}

//*******************************************************************************************************************

void DMD_Nepali::drawString(int cur_x_offset, int cur_y_offset, String message)
{
  int charLen = message.length() + 1;
  char *temp = new char[charLen];
  message.toCharArray(temp, charLen);
  drawString(cur_x_offset, cur_y_offset, temp, charLen);
  delete[] temp;
  temp = NULL;
}

//*******************************************************************************************************************

void DMD_Nepali::drawString(int cur_x_offset, int cur_y_offset, char *m, uint8_t length)
{
  int shiftx;
  char *next = &m[0];
  int cur_x1 = cur_x + cur_x_offset;
  int cur_y1 = cur_y + cur_y_offset;

  while (length > 0)
  {
    if (*next == '\n')
    { // Check for new line
      bool pending_newline = true;
    }
    else
    {
      bool pending_newline = false;
    }

    if (width == 0) // Set display height and width
      width = dmd.width;
    if (height == 0)
      height = dmd.height;

    //Check if the character is English or Nepali
    if (*next == '}' || *next == '~')
    {
      if (*next == '}')
      {
        change_flag = 0;
      }
      else
      {
        change_flag = 1;
      }
      goto skip;
    }

    //Print shorter space-------------------------------------------------------------------------------
    else if (*next == ' ')
    {
      dmd.selectFont(font_nepali);
      for (int i = 0; i < height; i++)
      {
        dmd.drawChar(cur_x1, cur_y1 + i * 16, ' ');
      }
      cur_x1 += 2;
      char_width = 2;
      // this -> checkScroll();
    }

    //Select English Font if flag is 0-------------------------------------------------------------------
    else if (!change_flag)
    {
      dmd.selectFont(font_eng); // Change to english font
      cur_x1++;
      char_width = dmd.charWidth(*next) + 1;
      // this -> checkScroll();
      dmd.drawChar(cur_x1, cur_y1 + eng_y_offset, *next);
      char_width = dmd.charWidth(*next);
      cur_x1 += char_width + 1;
    }

    //Select Nepali Font if flag is 1----------------------------------------------------------------------
    else if (change_flag)
    {
      dmd.selectFont(font_nepali); // Change to nepali font
      char temp = *(next + 1);

      // Check if half letters are used ----------------------------------------------------------------
      if (((*next >= '5' && *next <= '[') || *next == 'r' || *next == 'n') && *(next + 1) != '}' && ((temp >= '^' && temp <= 'm') || temp == '!' || temp == '"'))
      {
        if (temp == '^')
        { //Aa-kaar
          char_width = dmd.charWidth(*next) + dmd.charWidth(temp);
          // this -> checkScroll();
          dmd.drawChar(cur_x1, cur_y1 + 0, *next);
          cur_x1 += dmd.charWidth(*next);
          dmd.drawChar(cur_x1, cur_y1 + 0, temp);
          cur_x1 += dmd.charWidth(temp);
        }

        else if (temp == '!' || temp == '"' || temp == 'g' || temp == 'h')
        { // Chandrabindu, Sirbindhu, aeE-kaar and Aouu-kaar
          char_width = dmd.charWidth(*next);
          // this -> checkScroll();
          dmd.drawChar(cur_x1, cur_y1 + 0, *next);
          if (*next == '5' || *next == '@' || *next == 'K' || *next == 'Y')
          {
            cur_x1 += (dmd.charWidth(*next) - 11);
            dmd.drawChar(cur_x1, cur_y1 - 12, temp);
            cur_x1 += (dmd.charWidth(temp) + 3);
          }
          else
          {
            cur_x1 += (dmd.charWidth(*next) - 8);
            dmd.drawChar(cur_x1, cur_y1 - 12, temp);
            cur_x1 += dmd.charWidth(temp);
          }
        }

        else if (temp == '_')
        { // I-kaar
          char_width = dmd.charWidth(*next) + dmd.charWidth('^');
          // this -> checkScroll();

          dmd.drawChar(cur_x1, cur_y1 + 0, '^');
          cur_x1 += dmd.charWidth('^');
          dmd.drawChar(cur_x1, cur_y1 + 0, *next);
          cur_x1 -= 3;
          dmd.drawChar(cur_x1, cur_y1 - 12, temp);
          cur_x1 += (dmd.charWidth(*next) + 3);
        }

        else if (temp == '`' || temp == 'k' || temp == 'l')
        { // Ii-kaar, O-kaar and Au-kaar
          char_width = dmd.charWidth(*next) + dmd.charWidth('^');
          // this -> checkScroll();

          dmd.drawChar(cur_x1, cur_y1 + 0, *next);
          cur_x1 = cur_x1 + dmd.charWidth(*next);
          dmd.drawChar(cur_x1, cur_y1 + 0, '^');
          cur_x1 += (dmd.charWidth('^') - 8);
          dmd.drawChar(cur_x1, cur_y1 - 12, temp);
          cur_x1 += dmd.charWidth(temp);
        }

        else if (temp == 'a' || temp == 'c')
        { // U-kaar
          char_width = dmd.charWidth(*next);
          // this -> checkScroll();

          dmd.drawChar(cur_x1, cur_y1 + 0, *next);
          if (*next == '5' || *next == '@' || *next == 'K' || *next == 'Y')
          {
            cur_x1 += (dmd.charWidth(*next) - 11);
            dmd.drawChar(cur_x1, cur_y1 + 12, temp);
            cur_x1 += (dmd.charWidth(temp) + 3);
          }
          else
          {
            cur_x1 += (dmd.charWidth(*next) - 8);
            dmd.drawChar(cur_x1, cur_y1 + 12, temp);
            cur_x1 += dmd.charWidth(temp);
          }
        }

        else if (temp == 'b')
        { // UU-kaar
          char_width = dmd.charWidth(*next);
          // this -> checkScroll();

          dmd.drawChar(cur_x1, cur_y1 + 0, *next);
          if (*next == '5' || *next == '@' || *next == 'K' || *next == 'Y')
          {
            cur_x1 += (dmd.charWidth(*next) - 8);
            dmd.drawChar(cur_x1, cur_y1 + 12, temp);
            cur_x1 += (dmd.charWidth(temp));
          }
          else
          {
            cur_x1 += (dmd.charWidth(*next) - 7);
            dmd.drawChar(cur_x1, cur_y1 + 12, temp);
            cur_x1 += (dmd.charWidth(temp) - 1);
          }
        }

        else if (temp == 'm')
        { // Khutta kateko (half letter)
          if ((*(next + 2) != '}' && *(next + 2) >= '5' && *(next + 2) <= '[') && (*next != 'B' && *next != '?' && *next != '@'))
          { // if followed by another character
            int cur_var = 0;

            if (*(next + 2) == 'P' && (*next == '5' || *next == 'L' || *next == 'M' || *next == 'J'))
            { // if followed by Ra
              if (*next == '5')
              {
                *(next + 2) = 'S';
              }
              else if (*next == 'J')
              {
                *(next + 2) = 'T';
              }
              else if (*next == 'L')
              {
                *(next + 2) = 'Q';
              }
              else if (*next == 'M')
              {
                *(next + 2) = 'I';
              }
              next++;
              length--;
              goto skip;
            }
            else if (*(next + 2) == 'G' && (*next == 'F'))
            { // if Dha followed by dha
              *(next + 2) = 'n';
              next++;
              length--;
              goto skip;
            }
            else if (*(next + 2) == 'D' && (*next == '5'))
            { // if Ka followed by Ta
              *(next + 2) = 'i';
              next++;
              length--;
              goto skip;
            }
            else if ((*next == '5' && *(next + 2) == 'W') || (*next == 'D' && *(next + 2) == 'P') || (*next == '<' && *(next + 2) == '>'))
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
              next++;
              length--;
              goto skip;
            }
            else if (*next == 'K' || *next == '5' || *next == 'N')
            {
              cur_var = dmd.charWidth(*next) - 2;
              shiftx = 1;
            }
            else if (*next == 'V')
            {
              cur_var = dmd.charWidth(*next) - 4;
              shiftx = -1;
            }
            else if (*next == 'P')
            {
              cur_var = 0;
              shiftx = 0;
              r_flag = true;
              next++;
              length--;
              goto skip;
            }
            else
            {
              cur_var = dmd.charWidth(*next) - 3;
              shiftx = 0;
            }

            if (*(next + 3) == '_')
            { // if I-kaar
              char_width = dmd.charWidth('^') + cur_var + dmd.charWidth(*(next + 2));
              // this -> checkScroll();

              dmd.drawChar(cur_x1, cur_y1 + 0, '^');
              cur_x1 += dmd.charWidth('^');
              dmd.drawChar(cur_x1, cur_y1 + 0, *next);
              if (*next == 'K' || *next == '5' || *next == 'N' || *next == '<')
              {
                cur_x1 += (dmd.charWidth(*next) - 2);
              }
              else if (*next == 'V')
              {
                cur_x1 += (dmd.charWidth(*next) - 4);
              }
              else
              {
                cur_x1 += (dmd.charWidth(*next) - 3);
              }
              dmd.drawChar(cur_x1, cur_y1 + 0, '#');
              cur_x1 += 1;
              dmd.drawChar(cur_x1, cur_y1 + 0, *(next + 2));
              cur_x1 -= (shiftx + 7);
              dmd.drawChar(cur_x1, cur_y1 - 12, ']');
              cur_x1 += (dmd.charWidth(']') + 5 + shiftx);
              next += 2;
              length -= 2;
            }
            else
            { // if no I-kaar
              char_width = cur_var + dmd.charWidth(*(next + 2));
              // this -> checkScroll();

              dmd.drawChar(cur_x1, cur_y1 + 0, *next);
              if (*next == 'K' || *next == '5' || *next == 'N' || *next == '<')
              {
                cur_x1 += (dmd.charWidth(*next) - 2);
              }
              else if (*next == 'V')
              {
                cur_x1 += (dmd.charWidth(*next) - 4);
              }
              else
              {
                cur_x1 += (dmd.charWidth(*next) - 3);
              }
              dmd.drawChar(cur_x1, cur_y1 + 0, '#');
              cur_x1 += 1;
            }
          }
          else
          { // if not followed by another character
            char_width = dmd.charWidth(*next);
            // this -> checkScroll();

            dmd.drawChar(cur_x1, cur_y1 + 0, *next);
            if (*next == '5' || *next == '@' || *next == 'K' || *next == 'Y')
            {
              cur_x1 += (dmd.charWidth(*next) - 8);
              dmd.drawChar(cur_x1, cur_y1 + 12, temp);
              cur_x1 += (dmd.charWidth(temp));
            }
            else
            {
              cur_x1 += (dmd.charWidth(*next) - 7);
              dmd.drawChar(cur_x1, cur_y1 + 12, temp);
              cur_x1 += (dmd.charWidth(temp) - 1);
            }
          }
        }
        next++;
        length--;
      }

      // Not followed by half letters --------------------------------------------------------------
      else
      {
        char_width = dmd.charWidth(*next);
        // this -> checkScroll();

        dmd.drawChar(cur_x1, cur_y1 + 0, *next);
        cur_x1 += dmd.charWidth(*next);
      }
    }
  skip:

    next++;
    length--;

    // If half R appears ------------- --------------------------------------------------------------
    if (r_flag == true)
    {
      r_count++;
    }

    if (r_count > 1)
    {
      char_width = 1;
      // this -> checkScroll();
      dmd.drawChar(cur_x1 - 2, -12, 'd');
      dmd.drawChar(cur_x1, 0, 'e');
      cur_x1++;
      r_flag = false;
      r_count = 0;
    }
    //delay(scroll_delay);
  }
}

//******************************************************************************************************************

void DMD_Nepali::printString(char *m, uint8_t length = 0)
{
  int shiftx;
  char *next = &m[0];

  while (length > 0)
  {
    if (*next == '\n')
    { // Check for new line
      bool pending_newline = true;
    }
    else
    {
      bool pending_newline = false;
    }

    if (width == 0) // Set display height and width
      width = dmd.width;
    if (height == 0)
      height = dmd.height;

    //Check if the character is English or Nepali
    if (*next == '}' || *next == '~')
    {
      if (*next == '}')
      {
        change_flag = 0;
      }
      else
      {
        change_flag = 1;
      }
      goto skip;
    }

    //Print shorter space-------------------------------------------------------------------------------
    else if (*next == ' ')
    {
      dmd.selectFont(font_nepali);
      for (int i = 0; i < height; i++)
      {
        dmd.drawChar(cur_x, cur_y + i * 16, ' ');
      }
      cur_x += 2;
      char_width = 2;
      this->checkScroll();
    }

    //Select English Font if flag is 0-------------------------------------------------------------------
    else if (!change_flag)
    {
      dmd.selectFont(font_eng); // Change to english font
      cur_x++;
      char_width = dmd.charWidth(*next) + 1;
      this->checkScroll();
      dmd.drawChar(cur_x, eng_y_offset, *next);
      char_width = dmd.charWidth(*next);
      cur_x += char_width + 1;
    }

    //Select Nepali Font if flag is 1----------------------------------------------------------------------
    else if (change_flag)
    {
      dmd.selectFont(font_nepali); // Change to nepali font
      char temp = *(next + 1);

      // Check if half letters are used ----------------------------------------------------------------
      if (((*next >= '5' && *next <= '[') || *next == 'r' || *next == 'n') && *(next + 1) != '}' && ((temp >= '^' && temp <= 'm') || temp == '!' || temp == '"'))
      {
        if (temp == '^')
        { //Aa-kaar
          char_width = dmd.charWidth(*next) + dmd.charWidth(temp);
          this->checkScroll();
          dmd.drawChar(cur_x, 0, *next);
          cur_x += dmd.charWidth(*next);
          dmd.drawChar(cur_x, 0, temp);
          cur_x += dmd.charWidth(temp);
        }

        else if (temp == '!' || temp == '"' || temp == 'g' || temp == 'h')
        { // Chandrabindu, Sirbindhu, E-kaar and Ai-kaar
          char_width = dmd.charWidth(*next);
          this->checkScroll();
          dmd.drawChar(cur_x, 0, *next);
          if (*next == '5' || *next == '@' || *next == 'K' || *next == 'Y')
          {
            cur_x += (dmd.charWidth(*next) - 11);
            dmd.drawChar(cur_x, -12, temp);
            cur_x += (dmd.charWidth(temp) + 3);
          }
          else
          {
            cur_x += (dmd.charWidth(*next) - 8);
            dmd.drawChar(cur_x, -12, temp);
            cur_x += dmd.charWidth(temp);
          }
        }

        else if (temp == '_')
        { // I-kaar
          char_width = dmd.charWidth(*next) + dmd.charWidth('^');
          this->checkScroll();

          dmd.drawChar(cur_x, 0, '^');
          cur_x += dmd.charWidth('^');
          dmd.drawChar(cur_x, 0, *next);
          cur_x -= 3;
          dmd.drawChar(cur_x, -12, temp);
          cur_x += (dmd.charWidth(*next) + 3);
        }

        else if (temp == '`' || temp == 'k' || temp == 'l')
        { // Ii-kaar, O-kaar and Au-kaar
          char_width = dmd.charWidth(*next) + dmd.charWidth('^');
          this->checkScroll();

          dmd.drawChar(cur_x, 0, *next);
          cur_x = cur_x + dmd.charWidth(*next);
          dmd.drawChar(cur_x, 0, '^');
          cur_x += (dmd.charWidth('^') - 8);
          dmd.drawChar(cur_x, -12, temp);
          cur_x += dmd.charWidth(temp);
        }

        else if (temp == 'a' || temp == 'c')
        { // U-kaar
          char_width = dmd.charWidth(*next);
          this->checkScroll();

          dmd.drawChar(cur_x, 0, *next);
          if (*next == '5' || *next == '@' || *next == 'K' || *next == 'Y')
          {
            cur_x += (dmd.charWidth(*next) - 11);
            dmd.drawChar(cur_x, 12, temp);
            cur_x += (dmd.charWidth(temp) + 3);
          }
          else
          {
            cur_x += (dmd.charWidth(*next) - 8);
            dmd.drawChar(cur_x, 12, temp);
            cur_x += dmd.charWidth(temp);
          }
        }

        else if (temp == 'b')
        { // UU-kaar
          char_width = dmd.charWidth(*next);
          this->checkScroll();

          dmd.drawChar(cur_x, 0, *next);
          if (*next == '5' || *next == '@' || *next == 'K' || *next == 'Y')
          {
            cur_x += (dmd.charWidth(*next) - 8);
            dmd.drawChar(cur_x, 12, temp);
            cur_x += (dmd.charWidth(temp));
          }
          else
          {
            cur_x += (dmd.charWidth(*next) - 7);
            dmd.drawChar(cur_x, 12, temp);
            cur_x += (dmd.charWidth(temp) - 1);
          }
        }

        else if (temp == 'm')
        { // Khutta kateko (half letter)
          if ((*(next + 2) != '}' && *(next + 2) >= '5' && *(next + 2) <= '[') && (*next != 'B' && *next != '?' && *next != '@'))
          { // if followed by another character
            int cur_var = 0;

            if (*(next + 2) == 'P' && (*next == '5' || *next == 'L' || *next == 'M' || *next == 'J'))
            { // if followed by Ra
              if (*next == '5')
              {
                *(next + 2) = 'S';
              }
              else if (*next == 'J')
              {
                *(next + 2) = 'T';
              }
              else if (*next == 'L')
              {
                *(next + 2) = 'Q';
              }
              else if (*next == 'M')
              {
                *(next + 2) = 'I';
              }
              next++;
              length--;
              goto skip;
            }
            else if (*(next + 2) == 'G' && (*next == 'F'))
            { // if Dha followed by dha
              *(next + 2) = 'n';
              next++;
              length--;
              goto skip;
            }
            else if (*(next + 2) == 'D' && (*next == '5'))
            { // if Ka followed by Ta
              *(next + 2) = 'i';
              next++;
              length--;
              goto skip;
            }
            else if ((*next == '5' && *(next + 2) == 'W') || (*next == 'D' && *(next + 2) == 'P') || (*next == '<' && *(next + 2) == '>'))
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
              next++;
              length--;
              goto skip;
            }
            else if (*next == 'K' || *next == '5' || *next == 'N')
            {
              cur_var = dmd.charWidth(*next) - 2;
              shiftx = 1;
            }
            else if (*next == 'V')
            {
              cur_var = dmd.charWidth(*next) - 4;
              shiftx = -1;
            }
            else if (*next == 'P')
            {
              cur_var = 0;
              shiftx = 0;
              r_flag = true;
              next++;
              length--;
              goto skip;
            }
            else
            {
              cur_var = dmd.charWidth(*next) - 3;
              shiftx = 0;
            }

            if (*(next + 3) == '_')
            { // if I-kaar
              char_width = dmd.charWidth('^') + cur_var + dmd.charWidth(*(next + 2));
              this->checkScroll();

              dmd.drawChar(cur_x, 0, '^');
              cur_x += dmd.charWidth('^');
              dmd.drawChar(cur_x, 0, *next);
              if (*next == 'K' || *next == '5' || *next == 'N' || *next == '<')
              {
                cur_x += (dmd.charWidth(*next) - 2);
              }
              else if (*next == 'V')
              {
                cur_x += (dmd.charWidth(*next) - 4);
              }
              else
              {
                cur_x += (dmd.charWidth(*next) - 3);
              }
              dmd.drawChar(cur_x, 0, '#');
              cur_x += 1;
              dmd.drawChar(cur_x, 0, *(next + 2));
              cur_x -= (shiftx + 7);
              dmd.drawChar(cur_x, -12, ']');
              cur_x += (dmd.charWidth(']') + 5 + shiftx);
              next += 2;
              length -= 2;
            }
            else
            { // if no I-kaar
              char_width = cur_var + dmd.charWidth(*(next + 2));
              this->checkScroll();

              dmd.drawChar(cur_x, 0, *next);
              if (*next == 'K' || *next == '5' || *next == 'N' || *next == '<')
              {
                cur_x += (dmd.charWidth(*next) - 2);
              }
              else if (*next == 'V')
              {
                cur_x += (dmd.charWidth(*next) - 4);
              }
              else
              {
                cur_x += (dmd.charWidth(*next) - 3);
              }
              dmd.drawChar(cur_x, 0, '#');
              cur_x += 1;
            }
          }
          else
          { // if not followed by another character
            char_width = dmd.charWidth(*next);
            this->checkScroll();

            dmd.drawChar(cur_x, 0, *next);
            if (*next == '5' || *next == '@' || *next == 'K' || *next == 'Y')
            {
              cur_x += (dmd.charWidth(*next) - 8);
              dmd.drawChar(cur_x, 12, temp);
              cur_x += (dmd.charWidth(temp));
            }
            else
            {
              cur_x += (dmd.charWidth(*next) - 7);
              dmd.drawChar(cur_x, 12, temp);
              cur_x += (dmd.charWidth(temp) - 1);
            }
          }
        }
        next++;
        length--;
      }

      // Not followed by half letters --------------------------------------------------------------
      else
      {
        char_width = dmd.charWidth(*next);
        this->checkScroll();

        dmd.drawChar(cur_x, 0, *next);
        cur_x += dmd.charWidth(*next);
      }
    }
  skip:

    next++;
    length--;

    // If half R appears ------------- --------------------------------------------------------------
    if (r_flag == true)
    {
      r_count++;
    }

    if (r_count > 1)
    {
      char_width = 1;
      this->checkScroll();
      dmd.drawChar(cur_x - 2, -12, 'd');
      dmd.drawChar(cur_x, 0, 'e');
      cur_x++;
      r_flag = false;
      r_count = 0;
    }
    delay(scroll_delay);
  }
}

//******************************************************************************************************************

void DMD_Nepali::scrollString(char *m, uint8_t length = 0)
{
  int count = 0;
  int shiftx;
  m[length + 1] = ' ';
  m[length + 2] = 0;
  char *next = &m[0];

  while (length > 0 && *next != 0)
  {
    if (Serial.available())
      return;
    cur_x = width - 1;

    if (*next == '\n')
    { // Check for new line
      bool pending_newline = true;
    }
    else
    {
      bool pending_newline = false;
    }

    if (width == 0) // Set display height and width
      width = dmd.width;
    if (height == 0)
      height = dmd.height;

    //Check if the character is English or Nepali
    if (*next == '}' || *next == '~')
    {
      if (*next == '}')
      {
        change_flag = 0;
      }
      else
      {
        change_flag = 1;
      }
      goto skip;
    }

    //Print shorter space-------------------------------------------------------------------------------
    else if (*next == ' ')
    {
      dmd.selectFont(font_nepali);
      for (int i = 0; i < height; i++)
      {
        dmd.drawChar(cur_x, cur_y + i * 16, ' ');
      }
      char_width = 4;
      while ((width - cur_x) <= char_width)
      {
        scrollX(-1);
        delay(scroll_delay);
      }
    }

    //Select English Font if flag is 0-------------------------------------------------------------------
    else if (!change_flag)
    {
      dmd.selectFont(font_eng);              // Change to english font
      char_width = dmd.charWidth(*next) + 1; // *******************************
      scrollX(-1);
      cur_x++;
      while ((width - cur_x) <= char_width)
      {
        dmd.drawChar(cur_x, eng_y_offset, *next);
        scrollX(-1);
        delay(scroll_delay);
      }
      cur_x--;
    }

    //Select Nepali Font if flag is 1----------------------------------------------------------------------
    else if (change_flag)
    {
      dmd.selectFont(font_nepali); // Change to nepali font
      char temp = *(next + 1);
      // Check if half letters are used ----------------------------------------------------------------
      if (((*next >= '5' && *next <= '[') || *next == 'r' || *next == 'n') && temp != '}' && ((temp >= '^' && temp <= 'm') || temp == '!' || temp == '"'))
      {
        if (temp == '^')
        { //Aa-kaar
          char_width = dmd.charWidth(*next) + dmd.charWidth(temp);
          while ((width - cur_x) <= char_width)
          {
            dmd.drawChar(cur_x, 0, *next);
            cur_x += dmd.charWidth(*next);
            dmd.drawChar(cur_x, 0, temp);
            cur_x += dmd.charWidth(temp);

            cur_x -= char_width;
            scrollX(-1);
            delay(scroll_delay);
          }
        }

        else if (temp == '!' || temp == '"' || temp == 'g' || temp == 'h')
        { // Chandrabindu, Sirbindhu, E-kaar and Ai-kaar
          char_width = dmd.charWidth(*next);
          while ((width - cur_x) <= char_width)
          {
            dmd.drawChar(cur_x, 0, *next);
            if (*next == '5' || *next == '@' || *next == 'K' || *next == 'Y')
            {
              cur_x += (dmd.charWidth(*next) - 10);
              dmd.drawChar(cur_x, -12, temp);
              cur_x += (dmd.charWidth(temp) + 2);
            }
            else
            {
              cur_x += (dmd.charWidth(*next) - 8);
              dmd.drawChar(cur_x, -12, temp);
              cur_x += dmd.charWidth(temp);
            }
            cur_x -= char_width;
            scrollX(-1);
            delay(scroll_delay);
          }
        }

        else if (temp == '_')
        { // I-kaar
          char_width = dmd.charWidth(*next) + dmd.charWidth('^');
          while ((width - cur_x) <= char_width)
          {
            dmd.drawChar(cur_x, 0, '^');
            cur_x += dmd.charWidth('^');
            dmd.drawChar(cur_x, 0, *next);
            cur_x -= 3;
            dmd.drawChar(cur_x, -12, temp);
            cur_x += (dmd.charWidth(*next) + 3);

            cur_x -= char_width;
            scrollX(-1);
            delay(scroll_delay);
          }
        }

        else if (temp == '`' || temp == 'k' || temp == 'l')
        { // Ii-kaar, O-kaar and Au-kaar
          char_width = dmd.charWidth(*next) + dmd.charWidth('^');
          while ((width - cur_x) <= char_width)
          {
            dmd.drawChar(cur_x, 0, *next);
            cur_x = cur_x + dmd.charWidth(*next);
            dmd.drawChar(cur_x, 0, '^');
            cur_x += (dmd.charWidth('^') - 8);
            dmd.drawChar(cur_x, -12, temp);
            cur_x += dmd.charWidth(temp);

            cur_x -= char_width;
            scrollX(-1);
            delay(scroll_delay);
          }
        }

        else if (temp == 'a' || temp == 'c')
        { // U-kaar
          char_width = dmd.charWidth(*next);
          while ((width - cur_x) <= char_width)
          {
            dmd.drawChar(cur_x, 0, *next);
            if (*next == '5' || *next == '@' || *next == 'K' || *next == 'Y')
            {
              cur_x += (dmd.charWidth(*next) - 11);
              dmd.drawChar(cur_x, 12, temp);
              cur_x += (dmd.charWidth(temp) + 3);
            }
            else
            {
              cur_x += (dmd.charWidth(*next) - 8);
              dmd.drawChar(cur_x, 12, temp);
              cur_x += dmd.charWidth(temp);
            }

            cur_x -= char_width;
            scrollX(-1);
            delay(scroll_delay);
          }
        }

        else if (temp == 'b')
        { // UU-kaar
          char_width = dmd.charWidth(*next);
          while ((width - cur_x) <= char_width)
          {
            dmd.drawChar(cur_x, 0, *next);
            if (*next == '5' || *next == '@' || *next == 'K' || *next == 'Y')
            {
              cur_x += (dmd.charWidth(*next) - 8);
              dmd.drawChar(cur_x, 12, temp);
              cur_x += (dmd.charWidth(temp));
            }
            else
            {
              cur_x += (dmd.charWidth(*next) - 7);
              dmd.drawChar(cur_x, 12, temp);
              cur_x += (dmd.charWidth(temp) - 1);
            }

            cur_x -= char_width;
            scrollX(-1);
            delay(scroll_delay);
          }
        }

        else if (temp == 'm')
        { // Khutta kateko (half letter)
          if ((*(next + 2) != '}' && *(next + 2) >= '5' && *(next + 2) <= '[') && (*next != 'B' && *next != '?' && *next != '@'))
          { // if followed by another character
            int cur_var = 0;

            if (*(next + 2) == 'P' && (*next == '5' || *next == '7' || *next == 'L' || *next == 'M' || *next == 'J'))
            { // if followed by Ra
              if (*next == '5')
              {
                *(next + 2) = 'S';
              }
              else if (*next == '7')
              {
                *(next + 2) = 'f';
              }
              else if (*next == 'J')
              {
                *(next + 2) = 'T';
              }
              else if (*next == 'L')
              {
                *(next + 2) = 'Q';
              }
              else if (*next == 'M')
              {
                *(next + 2) = 'I';
              }
              next++;
              length--;
              goto skip;
            }
            else if (*(next + 2) == 'G' && (*next == 'F'))
            { // if Dha followed by dha
              *(next + 2) = 'n';
              next++;
              length--;
              goto skip;
            }
            else if (*(next + 2) == 'D' && (*next == '5'))
            { // if Ka followed by Ta
              *(next + 2) = 'i';
              next++;
              length--;
              goto skip;
            }
            else if ((*next == '5' && *(next + 2) == 'W') || (*next == 'D' && *(next + 2) == 'P') || (*next == '<' && *(next + 2) == '>'))
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
              next++;
              length--;
              goto skip;
            }
            else if (*next == 'K' || *next == '5' || *next == 'N' || *next == '<')
            {
              cur_var = -2;
              shiftx = 1;
              add_width = 1;
            }
            else if (*next == 'V')
            {
              cur_var = -4;
              shiftx = -1;
              add_width = 0;
            }
            else if (*next == 'P')
            {
              cur_var = 0;
              shiftx = 0;
              r_flag = true;
              next++;
              length--;
              goto skip;
            }
            else
            {
              cur_var = -3;
              shiftx = 0;
              add_width = 0;
            }

            if (*(next + 3) == '_')
            {
              char_width = dmd.charWidth(*next) + cur_var + dmd.charWidth('^') + dmd.charWidth(*(next + 2)) + add_width;
              while ((width - cur_x) <= char_width)
              {
                dmd.drawChar(cur_x, 0, '^');
                cur_x += dmd.charWidth('^');
                dmd.drawChar(cur_x, 0, *next);
                cur_x += dmd.charWidth(*next) + cur_var;
                dmd.drawChar(cur_x, 0, '#');
                cur_x += dmd.charWidth('#') - 2;
                dmd.drawChar(cur_x, 0, *(next + 2));
                cur_x -= (shiftx + 7);
                dmd.drawChar(cur_x, -12, ']');
                cur_x += (dmd.charWidth(']') + 4 + shiftx);

                cur_x -= char_width;
                scrollX(-1);
                delay(scroll_delay);
              }
              cur_x = width - 1;
              dmd.drawChar(cur_x, 0, '#');
              scrollX(-1);
              delay(scroll_delay);

              next += 2;
              length -= 2;
            }
            else
            { // if no I-kaar
              char_width = dmd.charWidth(*next) + cur_var;
              while ((width - cur_x) <= char_width)
              {
                dmd.drawChar(cur_x, 0, *next);
                scrollX(-1);
                delay(scroll_delay);
              }
              cur_x = width - 1;
              dmd.drawChar(cur_x, 0, '#');
              scrollX(-1);
              delay(scroll_delay);
            }
          }
          else
          { // if not followed by another character
            char_width = dmd.charWidth(*next);
            while ((width - cur_x) <= char_width)
            {
              dmd.drawChar(cur_x, 0, *next);
              if (*next == '5' || *next == '@' || *next == 'K' || *next == 'Y')
              {
                cur_x += (dmd.charWidth(*next) - 8);
                dmd.drawChar(cur_x, 12, temp);
                cur_x += (dmd.charWidth(temp));
              }
              else
              {
                cur_x += (dmd.charWidth(*next) - 7);
                dmd.drawChar(cur_x, 12, temp);
                cur_x += (dmd.charWidth(temp) - 1);
              }

              cur_x -= char_width;
              scrollX(-1);
              delay(scroll_delay);
            }
          }
        }
        next++;
        length--;
      }

      // Not followed by half letters --------------------------------------------------------------
      else
      {
        char_width = dmd.charWidth(*next);
        while ((width - cur_x) <= char_width)
        {
          dmd.drawChar(cur_x, 0, *next);
          scrollX(-1);
          delay(scroll_delay);
        }
      }
    }
  skip:

    next++;
    length--;

    // If half R appears ------------- --------------------------------------------------------------
    if (r_flag == true)
    {
      r_count++;
    }

    if (r_count > 1)
    {
      cur_x = width - 1;
      char_width = 1;
      while ((width - cur_x) <= char_width)
      {
        dmd.drawChar(cur_x - 2, -12, 'd');
        dmd.drawChar(cur_x, 0, 'e');
        cur_x++;

        cur_x -= char_width;
        scrollX(-1);
        delay(scroll_delay);
      }
      r_flag = false;
      r_count = 0;
    }
  }
  for (int i = 0; i <= (width - cur_x); i++)
  {
    scrollX(-1);
    delay(scroll_delay);
  }
}

//******************************************************************************************************************

void DMD_Nepali::scrollX(int scrollBy)
{
  if (abs(scrollBy) >= width)
  { // scrolling over the whole display!
    // scrolling will erase everything
    dmd.drawFilledBox(left, top, left + width - 1, top + height - 1, inverted ? GRAPHICS_ON : GRAPHICS_OFF);
  }
  else if (scrollBy < 0)
  { // Scroll left
    movePixels(left - scrollBy, top, left, top, width + scrollBy, height);
  }
  else
  { // Scroll right
    movePixels(left, top, left + scrollBy, top, width - scrollBy, height);
  }

  cur_x += scrollBy;
  while (cur_x < 0)
    cur_x += width;
  while (cur_x > width)
    cur_x -= width;
}

void DMD_Nepali::movePixels(unsigned int from_x, unsigned int from_y,
                            unsigned int to_x, unsigned int to_y,
                            unsigned int width, unsigned int height)
{
  // NB: This implementation is actually a copy-erase so
  // it uses more RAM than a real move implementation would
  // do (however bypasses issues around overlapping regions.)

  if (from_x >= dmd.width || from_y >= dmd.height || to_x >= dmd.width || to_y >= dmd.height)
    return;

  DMDFrame to_move = DMDFrame(width, height);
  DMDFrame temp = dmd.subFrame(from_x, from_y, width, height);
  to_move.copyFrame(temp, to_x, to_y);
  dmd.swapBuffers(to_move);
}

//******************************************************************************************************************

void DMD_Nepali::clear()
{
  this->reset();
  dmd.drawFilledBox(left, top, left + width, top + height, inverted ? GRAPHICS_ON : GRAPHICS_OFF);
}

//******************************************************************************************************************

void DMD_Nepali::reset()
{
  cur_x = 0;
  cur_y = 0;
  pending_newline = false;
}

void DMD_Nepali::drawBoldString(int cur_x_offset, int cur_y_offset, String message)
{
  int charLen = message.length() + 1;
  char *temp = new char[charLen];
  message.toCharArray(temp, charLen);
  drawBoldString(cur_x_offset, cur_y_offset, temp, charLen);
  delete[] temp;
  temp = NULL;
}

void DMD_Nepali::drawBoldString(int cur_x_offset, int cur_y_offset, char *m, uint8_t length)
{
  length--;
  //  Serial.println(length);
  dmd.selectFont(NepFont_Bold);
  int cur_x1 = cur_x + cur_x_offset;
  int cur_y1 = cur_y + cur_y_offset;
  char *next = &m[0];
  uint8_t cutter = 0;
  uint8_t kota_cutter = 0;
  // Serial.println("hello");
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
      dmd.drawChar(cur_x1, cur_y1, '\'');
      dmd.drawChar(cur_x1 + 2, cur_y1 - 16, 'd');
      cur_x1 += dmd.charWidth('\'');
      length--;
      kota_cutter++;
      goto terminal;
    }

    if (*next == '0')
    {
      dmd.drawChar(cur_x1, cur_y1, '/');
      dmd.drawChar(cur_x1 - 6, cur_y1 - 16, 'g');
      cur_x1 += dmd.charWidth('/');
      length--;
      kota_cutter++;
      goto terminal;
    }

    if (*(next + 2) == 'P' && (*next == '5' || *next == 'L' || *next == 'M' || *next == 'J') && *(next + 1) == 'm')
    { // if followed by Ra
      if (*next == '5')
      {
        *(next + 2) = 'S';
      }
      else if (*next == 'J')
      {
        *(next + 2) = 'T';
      }
      else if (*next == 'L')
      {
        *(next + 2) = 'Q';
      }
      else if (*next == 'M')
      {
        *(next + 2) = 'I';
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
        dmd.drawChar(cur_x1, cur_y1, '^');
        dmd.drawChar(cur_x1, cur_y1 - 16, '_');
        cur_x1 += dmd.charWidth('^');
        length--;
        kota_cutter = kota_cutter + 1;
      }
      dmd.drawChar(cur_x1, cur_y1, *(next + 2));

      if (*(next + 3) == 'g' || *(next + 3) == 'h')
      {
        if (*(next + 1) == 'h')
        {
          dmd.drawChar(cur_x1 + 2, cur_y1 - 17, *(next + 3));
        }
        else
        {
          dmd.drawChar(cur_x1 - 2, cur_y1 - 17, *(next + 3));
        }
        length--;
        kota_cutter = kota_cutter + 1;
      }

      if (*(next + 3) == 'a' || *(next + 3) == 'b')
      {
        dmd.drawChar(cur_x1, cur_y1 + 17, *(next + 3));
        length--;
        kota_cutter = kota_cutter + 1;
      }

      cur_x1 += dmd.charWidth(*(next + 2));
      length--;
      kota_cutter++;

      if (*(next + 3) == '`')
      {
        dmd.drawChar(cur_x1, cur_y1, '^');
        dmd.drawChar(cur_x1 - 10, cur_y1 - 16, '_');
        cur_x1 += dmd.charWidth('^');
        length--;
        kota_cutter = kota_cutter + 1;
      }

      if (*(next + 3) == 'k' || *(next + 3) == 'l')
      {
        dmd.drawChar(cur_x1, cur_y1, '^');
        if (*(next + 3) == 'l')
        {
          dmd.drawChar(cur_x1 - 4, cur_y1 - 17, *(next + 3));
        }
        else
        {
          dmd.drawChar(cur_x1 - 9, cur_y1 - 17, *(next + 3));
        }
        cur_x1 += dmd.charWidth('^');
        length--;
        kota_cutter = kota_cutter + 1;
      }

      if (*(next + 3) == '^')
      {
        dmd.drawChar(cur_x1, cur_y1, '^');

        cur_x1 += dmd.charWidth('^');
        length--;
        kota_cutter = kota_cutter + 1;
      }

      dmd.drawChar(cur_x1 - 8, cur_y1 - 17, 'd');
      length = length - 2;
      kota_cutter = kota_cutter + 2;

      goto terminal;
    }

    if (((*next == '@' || *next == 'A' || *next == 'B' || *next == '?' || *next == ';' || *next == '9' || *next == 'Y') && (*(next + 1) == 'm')) || ((*(next + 1) == 'm') && (*(next+2) == ' '))) //----------> for khutta kateko tah, ta, tha, cha, nga etc and last letter ko khutta kateko
    {
      dmd.drawChar(cur_x1, cur_y1, *next);
      int new_cur_x1 = cur_x1 + dmd.charWidth(*next) / 4;
      dmd.drawChar(new_cur_x1, cur_y1 + 17, *(next + 1));
      cur_x1 += dmd.charWidth(*next);
      // Serial.println("hello");
      length = length - 2;
      kota_cutter = kota_cutter + 2;
      goto terminal;
    }
    else if (*(next + 1) == 'm' && *(next + 2) != ' ') //-----------------> khutta kateko (for the middle alphabet (or not the last one))(adhi na, adhi ka etc);
    {
      int8_t calliber = 0;
      if (*(next + 3) == '_')
      {
        dmd.drawChar(cur_x1, cur_y1, '^');
        dmd.drawChar(cur_x1, cur_y1 - 16, '_');
        cur_x1 += dmd.charWidth('^');
        length--;
        kota_cutter = kota_cutter + 1;
      }

      dmd.drawChar(cur_x1, cur_y1, *next);
      if (*next == '6' || *next == '7' || *next == '8' || *next == ':' || *next == '<' || *next == '=' || *next == 'C' || *next == 'J' || *next == 'L' || *next == 'M' || *next == 'N' || *next == 'D' || *next == 'E' || *next == 'G' || *next == 'H' || *next == 'O' || *next == 'R' || *next == 'U' || *next == 'V' || *next == 'W' || *next == 'X' || *next == '>' || *next == '%')
        calliber = -6;

      if (*next == '5' || *next == 'K')
        calliber = -((dmd.charWidth(*next) / 3));
      int new_cur_x1 = cur_x1 + dmd.charWidth(*next) + calliber;
      dmd.drawChar(new_cur_x1, cur_y1, *(next + 2));

      if (*(next + 3) == 'a' || *(next + 3) == 'b')
      {
        dmd.drawChar(new_cur_x1, cur_y1 + 17, *(next + 3));
        length--;
        kota_cutter = kota_cutter + 1;
      }

      if (*(next + 3) == '!' || *(next + 3) == '\"')
      {
        if (*(next + 3) == '!')
        {
          dmd.drawChar(new_cur_x1, cur_y1 - 16, *(next + 3));
        }
        else
        {
          dmd.drawChar(new_cur_x1 + 5, cur_y1 - 17, *(next + 3));
        }
        length--;
        kota_cutter = kota_cutter + 1;
      }

      if (*(next + 3) == 'g' || *(next + 3) == 'h')
      {
        if (*(next + 3) == 'h')
        {
          dmd.drawChar(new_cur_x1 + 2, cur_y1 - 17, *(next + 3));
        }
        else
        {
          dmd.drawChar(new_cur_x1 - 2, cur_y1 - 17, *(next + 3));
        }
        length--;
        kota_cutter = kota_cutter + 1;
      }

      cur_x1 = new_cur_x1 + dmd.charWidth(*(next + 2));
      if (*(next + 3) == '`')
      {
        dmd.drawChar(cur_x1, cur_y1, '^');
        dmd.drawChar(cur_x1 - 10, cur_y1 - 16, '_');
        cur_x1 += dmd.charWidth('^');
        length--;
        kota_cutter = kota_cutter + 1;
      }


      if (*(next + 3) == 'k' || *(next + 3) == 'l')
      {
        dmd.drawChar(cur_x1, cur_y1, '^');
        if (*(next + 3) == 'l')
        {
          dmd.drawChar(cur_x1 - 4, cur_y1 - 17, *(next + 3));
        }
        else
        {
          dmd.drawChar(cur_x1 - 9, cur_y1 - 17, *(next + 3));
        }
        cur_x1 += dmd.charWidth('^');
        length--;
        kota_cutter = kota_cutter + 1;
      }

      length = length - 3;
      kota_cutter = kota_cutter + 3;
      goto terminal;
    }

    if (*(next + 1) == '!' || *(next + 1) == '^' || *(next + 1) == '/"' || *(next + 1) == 'g' || *(next + 1) == 'h' || *(next + 1) == 'a' || *(next + 1) == 'b' || *(next + 1) == 'k' || *(next + 1) == 'c' || *(next + 1) == 'l' || *(next + 1) == '_' || *(next + 1) == '`')
    {

      if (*(next + 1) == '_') ///-------------------- baki xa ajhai (chandrabindu, sirbindhu, rikar)
      {
        dmd.drawChar(cur_x1, cur_y1, '^');
        dmd.drawChar(cur_x1, cur_y1 - 16, '_');
        cur_x1 += dmd.charWidth('^');
        length--;
        kota_cutter = kota_cutter + 1;
      }

      dmd.drawChar(cur_x1, cur_y1, *next);

      if (*(next + 1) == '!' || *(next + 1) == '\"')
      {
        if (*(next + 1) == '!')
        {
          dmd.drawChar(cur_x1, cur_y1 - 16, *(next + 1));
          if (*(next + 2) == 'a' || *(next + 2) == 'b')
          {
            dmd.drawChar(cur_x1 + dmd.charWidth(*(next)) - 5, cur_y1 + 16, *(next + 2));
          }
        }
        else
        {
          dmd.drawChar(cur_x1 + 5, cur_y1 - 17, *(next + 1));
        }
        length--;
        kota_cutter = kota_cutter + 1;
      }

      if (*(next + 1) == 'g' || *(next + 1) == 'h')
      {
        if (*(next + 1) == 'h')
        {
          dmd.drawChar(cur_x1 + 2, cur_y1 - 17, *(next + 1));
        }
        else
        {
          dmd.drawChar(cur_x1 - 2, cur_y1 - 17, *(next + 1));
        }
        length--;
        kota_cutter = kota_cutter + 1;
      }

      if (*(next + 1) == 'c')
      {
        dmd.drawChar((cur_x1 + ((dmd.charWidth(*next)) / 2) + 1), cur_y1 + 16, 'c');
        length--;
        kota_cutter++;
      }

      if (*(next + 1) == 'a' || *(next + 1) == 'b')
      {
        dmd.drawChar(cur_x1, cur_y1 + 17, *(next + 1));
        length--;
        kota_cutter = kota_cutter + 1;
      }

      cur_x1 += dmd.charWidth(*next);

      if (*(next + 1) == '`')
      {
        dmd.drawChar(cur_x1, cur_y1, '^');
        dmd.drawChar(cur_x1 - 10, cur_y1 - 16, '_');
        cur_x1 += dmd.charWidth('^');
        length--;
        kota_cutter = kota_cutter + 1;
      }

      if (*(next + 1) == 'k' || *(next + 1) == 'l')
      {
        dmd.drawChar(cur_x1, cur_y1, '^');
        if (*(next + 1) == 'l')
        {
          dmd.drawChar(cur_x1 - 4, cur_y1 - 17, *(next + 1));
        }
        else
        {
          dmd.drawChar(cur_x1 - 9, cur_y1 - 17, *(next + 1));
        }
        cur_x1 += dmd.charWidth('^');
        length--;
        kota_cutter = kota_cutter + 1;
      }

      if (*(next + 1) == '^')
      {
        dmd.drawChar(cur_x1, cur_y1, '^');
        if (*(next + 2) == '!')
        {
          dmd.drawChar(cur_x1 - 6, cur_y1 - 16, *(next + 2));
          length--;
          kota_cutter = kota_cutter + 1;
        }

        cur_x1 += dmd.charWidth('^');
        length--;
        kota_cutter = kota_cutter + 1;
      }

      length--;
      kota_cutter = kota_cutter + 1;
    }
    else
    {
      dmd.drawChar(cur_x1, cur_y1, *next);
      cur_x1 += dmd.charWidth(*next);
      length--;
      kota_cutter = kota_cutter + 1;
    }

    goto terminal;
  terminal:
    //     Serial.print("kota_cutter --> ");
    //     Serial.println(kota_cutter);
    //     Serial.print("length --> ");
    //     Serial.println(length);
    cutter = cutter + kota_cutter;
    next = &m[cutter];
    //     Serial.print(*(next));
    //     Serial.print(" ");
    //     Serial.print(*(next + 1));
    //     Serial.print(" ");
    //     Serial.println(*(next + 2));
  }
}
