#include "DMD_Nepali.h"

DMD_Nepali::DMD_Nepali(DMDFrame &dmd, int left, int top, int width, int height):
	dmd(dmd),
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

void DMD_Nepali::checkScroll(){
  while((cur_x > 0 && cur_x + char_width >= width) || pending_newline) {            // Need to wrap to new line
      if(pending_newline) {                                                         // No room, so just clear display
        clear();
      } else {                                                                      // Scroll characters horizontally
        scroll_flag = true;
        scroll_by = char_width - (width - cur_x - 1);
        scrollX(-scroll_by);
      }
      pending_newline = false;
    }
}

//******************************************************************************************************************

unsigned char DMD_Nepali::getCharacterCount(){
  if(char_count == 17){}
  Serial.print("in header count = ");
  Serial.println(char_count);
  return (char_count - 1);
}

//******************************************************************************************************************

char* DMD_Nepali::charMappingESP(char *MESSAGE){
  uint8_t i = 0;
  uint8_t j = 0;
  char_count = 0;
  
  while((int)MESSAGE[i] != 0){
    if(MESSAGE[i] <= 126){                       // if the character is english ascii value is positive
      if(i == 0 || MESSAGE[i-1] > 126){
        m[j] = '}';
        j++;
        char_count++;
      }
      m[j] = MESSAGE[i];
      j++;
      char_count++;
    }
    else if(MESSAGE[i] == 224){                                           // if character is nepali
      if(i == 0 || MESSAGE[i-1] <= 126){
        m[j] = '~';
        j++;
        char_count++;
      }
      int temp1 = (int)MESSAGE[i];
      int temp2 = (int)MESSAGE[i+1];
      int temp3 = (int)MESSAGE[i+2];
      if (temp1 == 224){
        if(temp2 == 164){
          m[j] = temp3 - 96;
          j++;  
          char_count++;   
        }
        else if (temp2 == 165){
          if (temp3 <= 144){
            m[j] = temp3 - 32;
          }
          else if(temp3 >= 164){
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

char* DMD_Nepali::charMapping(char *MESSAGE){	
  uint8_t i = 0;
  uint8_t j = 0;
  char_count = 0;
  
  while((int)MESSAGE[i] != 0){
    Serial.println((int)MESSAGE[i]);
    if(MESSAGE[i] = ' '){                           // if the character is space
      m[j] = ' ';
      j++;
      char_count++;
    }
    else if(MESSAGE[i] >= 0){                       // if the character is english ascii value is positive
      if(i == 0 || MESSAGE[i-1] < 0){
        m[j] = '}';
        j++;
        char_count++;
      }
      m[j] = MESSAGE[i];
      j++;
      char_count++;
    }
    else{                                           // if character is negative ascii starts from negative
      if(i == 0 || MESSAGE[i-1] >= 0){
        m[j] = '~';
        j++;
        char_count++;
      }
      int temp1 = (int)MESSAGE[i];
      int temp2 = (int)MESSAGE[i+1];
      int temp3 = (int)MESSAGE[i+2];
      if (temp1 == -32){
        if(temp2 == -92){
          m[j] = temp3 + 160;
          j++;  
          char_count++;   
        }
        else if (temp2 == -91){
          if (temp3 <= -112){
            m[j] = temp3 + 224;
          }
          else if(temp3 >= -92){
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

void DMD_Nepali::printString(char* m, uint8_t length = 0){
  int shiftx;
  char *next = &m[0];
  
  while(length) {
    if (*next == '\n'){                           // Check for new line
      bool pending_newline = true;
    } else{
      bool pending_newline = false;
    }

    if(width == 0)                                // Set display height and width
      width = dmd.width;
    if(height == 0)
      height = dmd.height;

//Check if the character is English or Nepali
    if (*next == '}' || *next == '~'){
      if(*next == '}'){
        change_flag = 0;
      } else{
        change_flag = 1;
      } 
      goto skip;
    }

//Print shorter space-------------------------------------------------------------------------------
    else if (*next == ' '){
      cur_x += 2;
      char_width = 2;
      this -> checkScroll();
    }
    
//Select English Font if flag is 0-------------------------------------------------------------------
    else if (!change_flag){
      dmd.selectFont(SystemFont5x7);              // Change to english font
      cur_x++;
      char_width = dmd.charWidth(*next) + 1;
      this -> checkScroll();
      dmd.drawChar(cur_x, 5, *next);
      char_width = dmd.charWidth(*next);
      cur_x += char_width + 1 ;
    }

//Select Nepali Font if flag is 1----------------------------------------------------------------------
    else if (change_flag){ 
      dmd.selectFont(NepFont1_16bit);             // Change to nepali font
      char temp = *(next+1);

      // Check if half letters are used ----------------------------------------------------------------
      if (((*next >= '5' && *next <='[') || *next == 'r') && *(next + 1) != '}' && ((temp >= '^' && temp <= 'm') || temp == '!' || temp == '"')) 
      {
        if (temp == '^'){                                                     //Aa-kaar
          char_width = dmd.charWidth(*next) + dmd.charWidth(temp);
          this -> checkScroll();
          dmd.drawChar(cur_x, 0, *next);
          cur_x += dmd.charWidth(*next);
          dmd.drawChar(cur_x, 0, temp);
          cur_x += dmd.charWidth(temp);
        }
        
        else if (temp == '!' || temp == '"' || temp =='g' || temp == 'h'){    // Chandrabindu, Sirbindhu, E-kaar and Ai-kaar
          char_width = dmd.charWidth(*next);
          this -> checkScroll();
          dmd.drawChar(cur_x, 0, *next);
          if (*next == '5' || *next == '@' || *next == 'K' || *next == 'Y'){
            cur_x += (dmd.charWidth(*next) - 11);
            dmd.drawChar(cur_x, -12, temp);
            cur_x += (dmd.charWidth(temp) + 3);
          }
          else{
            cur_x += (dmd.charWidth(*next) - 8);
            dmd.drawChar(cur_x, -12, temp);
            cur_x += dmd.charWidth(temp);
          }
        }
        
        else if (temp == '_'){                                                  // I-kaar
          char_width = dmd.charWidth(*next) + dmd.charWidth('^');
          this -> checkScroll();
          
          dmd.drawChar(cur_x, 0, '^');
          cur_x += dmd.charWidth('^'); 
          dmd.drawChar(cur_x, 0, *next);
          cur_x -= 3;
          dmd.drawChar(cur_x, -12, temp);
          cur_x += (dmd.charWidth(*next) + 3);
        }

        else if (temp == '`' || temp =='k' || temp == 'l'){                     // Ii-kaar, O-kaar and Au-kaar
          char_width = dmd.charWidth(*next) + dmd.charWidth('^');
          this -> checkScroll();
          
          dmd.drawChar(cur_x, 0, *next);
          cur_x = cur_x + dmd.charWidth(*next);
          dmd.drawChar(cur_x, 0, '^');
          cur_x += (dmd.charWidth('^') - 8);
          dmd.drawChar(cur_x, -12, temp);
          cur_x += dmd.charWidth(temp);
        }
        
        else if (temp == 'a' || temp == 'c'){                                   // U-kaar
          char_width = dmd.charWidth(*next);
          this -> checkScroll();

          dmd.drawChar(cur_x, 0, *next);
          if (*next == '5' || *next == '@' || *next == 'K' || *next == 'Y'){
            cur_x += (dmd.charWidth(*next) - 11);
            dmd.drawChar(cur_x, 12, temp);
            cur_x += (dmd.charWidth(temp) + 3);
          }
          else{
            cur_x += (dmd.charWidth(*next) - 8);
            dmd.drawChar(cur_x, 12, temp);
            cur_x += dmd.charWidth(temp);
          }
        }
        
        else if (temp =='b'){                                                    // UU-kaar
          char_width = dmd.charWidth(*next);
          this -> checkScroll();

          dmd.drawChar(cur_x, 0, *next);
          if (*next == '5' || *next == '@' || *next == 'K' || *next == 'Y'){
            cur_x += (dmd.charWidth(*next) - 8);
            dmd.drawChar(cur_x, 12, temp);
            cur_x += (dmd.charWidth(temp));
          }
          else{
            cur_x += (dmd.charWidth(*next) - 7);
            dmd.drawChar(cur_x, 12, temp);
            cur_x += (dmd.charWidth(temp) - 1);
          }
        }

        else if (temp == 'm'){                                                    // Khutta kateko (half letter)
          if (*(next+2) != '}' && *(next+2) >= '5' && *(next+2) <= '['){                // if followed by another character
            int cur_var = 0;

            if(*(next + 2) == 'P' && (*next == '5' || *next == 'L' || *next == 'M' || *next == 'J')){                 // if followed by Ra
              if (*next == '5'){
                *(next + 2) = 'S';
              } else if (*next == 'J'){
                *(next + 2) = 'T';
              } else if (*next == 'L'){
                *(next + 2) = 'Q';
              } else if (*next == 'M'){
                *(next + 2) = 'I';
              }
              next++;
              length--;
              goto skip;
            } else if((*next == '5' && *(next + 2) == 'W') || (*next == 'D' &&  *(next + 2) == 'P') || (*next == '<' && *(next + 2) == '>')){             // if kshya, tra and gya
              if (*next == '5'){
                *(next + 2) = 'Z';
              } else if (*next == 'D'){
                *(next + 2) = '[';
              } else if (*next == '<'){
                *(next + 2) = 'r';
              }
              next++;
              length--;
              goto skip;
            } else if(*next == 'K' || *next == '5' || *next == 'N'){
              cur_var = dmd.charWidth(*next) - 2;
              shiftx = 1;
            } else if(*next == 'V') {
              cur_var = dmd.charWidth(*next) - 4;
              shiftx = -1;
            } else if(*next == 'P') {
              cur_var = 0;
              shiftx = 0;
              r_flag = true;
              next++;
              length--;
              goto skip;
            } else{ 
              cur_var = dmd.charWidth(*next) - 3;
              shiftx = 0;
            }

            if (*(next+3) == '_') {                                                     // if I-kaar 
              char_width = dmd.charWidth('^') + cur_var + dmd.charWidth(*(next + 2));
              this -> checkScroll();
              
              dmd.drawChar(cur_x, 0, '^');
              cur_x += dmd.charWidth('^');
              dmd.drawChar(cur_x, 0, *next);
              if(*next == 'K' || *next == '5' || *next == 'N' || *next == '<'){
                cur_x += (dmd.charWidth(*next) - 2);
              } else if(*next == 'V'){
                cur_x += (dmd.charWidth(*next) - 4);
              }else {
                cur_x += (dmd.charWidth(*next) - 3);
              }
              dmd.drawChar(cur_x, 0, '#');
              cur_x += 1;
              dmd.drawChar(cur_x, 0, *(next+2));
              cur_x -= (shiftx + 7);
              dmd.drawChar(cur_x, -12, ']');
              cur_x += (dmd.charWidth(']') + 5 + shiftx);
              next += 2;
              length -= 2;
            }
            else {                                                                      // if no I-kaar
              char_width = cur_var + dmd.charWidth(*(next + 2));
              this -> checkScroll();

              dmd.drawChar(cur_x, 0, *next);
              if(*next == 'K' || *next == '5' || *next == 'N' || *next == '<'){
                cur_x += (dmd.charWidth(*next) - 2);
              } else if(*next == 'V'){
                cur_x += (dmd.charWidth(*next) - 4);
              }else {
                cur_x += (dmd.charWidth(*next) - 3);
              }
              dmd.drawChar(cur_x, 0, '#');
              cur_x += 1;
            }
          }
          else{                                                                       // if not followed by another character
            char_width = dmd.charWidth(*next);
            this -> checkScroll();

            dmd.drawChar(cur_x, 0, *next);
            if (*next == '5' || *next == '@' || *next == 'K' || *next == 'Y'){
              cur_x += (dmd.charWidth(*next) - 8);
              dmd.drawChar(cur_x, 12, temp);
              cur_x += (dmd.charWidth(temp));
            }
            else{
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
      else{                                                                            
        char_width = dmd.charWidth(*next);
        this -> checkScroll();

        dmd.drawChar(cur_x, 0, *next);
        cur_x += dmd.charWidth(*next);
      }
    }
    skip:

    next++;
    length--;

    // If half R appears ------------- --------------------------------------------------------------
    if(r_flag == true) {
      r_count++;
    }

    if(r_count > 1) {
      char_width = 1;
      this -> checkScroll();
      dmd.drawChar(cur_x - 2, -12, 'd');
      dmd.drawChar(cur_x , 0 , 'e');
      cur_x++;
      r_flag = false;
      r_count = 0;
    }
    delay(200);
  }  
}

//******************************************************************************************************************

void DMD_Nepali::scrollString(char* m, uint8_t length = 0, int cur_y = 5){
  int count = 0;
  int shiftx;
  char *next = &m[0];

  while(length > 0) {
    cur_x = width-1;

    if (*next == '\n'){                           // Check for new line
      bool pending_newline = true;
    } else{
      bool pending_newline = false;
    }

    if(width == 0)                                // Set display height and width
      width = dmd.width;
    if(height == 0)
      height = dmd.height;

//Check if the character is English or Nepali
    if (*next == '}' || *next == '~'){
      if(*next == '}'){
        change_flag = 0;
      } else{
        change_flag = 1;
      } 
      goto skip;
    }

//Print shorter space-------------------------------------------------------------------------------
    else if (*next == ' '){
      char_width = 4;
      while((width - cur_x) <= char_width){
        scrollX(-1);
        delay(scroll_delay);
      }
    }
    
//Select English Font if flag is 0-------------------------------------------------------------------
    else if (!change_flag){
      dmd.selectFont(SystemFont5x7);              // Change to english font
      char_width = dmd.charWidth(*next) + 1;      // *******************************
      scrollX(-1);
      cur_x++;
      while((width - cur_x) <= char_width){
        dmd.drawChar(cur_x, cur_y, *next);
        scrollX(-1);
        delay(scroll_delay);
      }
      cur_x--;
    }

//Select Nepali Font if flag is 1----------------------------------------------------------------------
    else if (change_flag){ 
      dmd.selectFont(NepFont1_16bit);             // Change to nepali font
      char temp = *(next+1);

      // Check if half letters are used ----------------------------------------------------------------
      if (((*next >= '5' && *next <='[') || *next == 'r') && *(next + 1) != '}' && ((temp >= '^' && temp <= 'm') || temp == '!' || temp == '"')) 
      {
        if (temp == '^'){                                                     //Aa-kaar
          char_width = dmd.charWidth(*next) + dmd.charWidth(temp);
          while((width - cur_x) <= char_width){
            dmd.drawChar(cur_x, cur_y - 5 , *next);
            cur_x += dmd.charWidth(*next);
            dmd.drawChar(cur_x, cur_y - 5, temp);
            cur_x += dmd.charWidth(temp);

            cur_x -= char_width;
            scrollX(-1);
            delay(scroll_delay);
          }
        }
        
        else if (temp == '!' || temp == '"' || temp =='g' || temp == 'h'){    // Chandrabindu, Sirbindhu, E-kaar and Ai-kaar
          char_width = dmd.charWidth(*next);
          while((width - cur_x) <= char_width){
            dmd.drawChar(cur_x, cur_y - 5, *next);
            if (*next == '5' || *next == '@' || *next == 'K' || *next == 'Y'){
              cur_x += (dmd.charWidth(*next) - 10);
              dmd.drawChar(cur_x, cur_y - 17, temp);
              cur_x += (dmd.charWidth(temp) + 2);
            }
            else{
              cur_x += (dmd.charWidth(*next) - 8);
              dmd.drawChar(cur_x, cur_y - 17, temp);
              cur_x += dmd.charWidth(temp);
            }
            cur_x -= char_width;
            scrollX(-1);
            delay(scroll_delay);
          }
          
        }
        
        else if (temp == '_'){                                                  // I-kaar
          char_width = dmd.charWidth(*next) + dmd.charWidth('^');
          while((width - cur_x) <= char_width){
            dmd.drawChar(cur_x, cur_y - 5, '^');
            cur_x += dmd.charWidth('^'); 
            dmd.drawChar(cur_x, cur_y - 5, *next);
            cur_x -= 3;
            dmd.drawChar(cur_x, cur_y - 17, temp);
            cur_x += (dmd.charWidth(*next) + 3);

            cur_x -= char_width;
            scrollX(-1);
            delay(scroll_delay);
          }
        }

        else if (temp == '`' || temp =='k' || temp == 'l'){                     // Ii-kaar, O-kaar and Au-kaar
          char_width = dmd.charWidth(*next) + dmd.charWidth('^');
          while((width - cur_x) <= char_width){
            dmd.drawChar(cur_x, cur_y - 5, *next);
            cur_x = cur_x + dmd.charWidth(*next);
            dmd.drawChar(cur_x, cur_y - 5, '^');
            cur_x += (dmd.charWidth('^') - 8);
            dmd.drawChar(cur_x, cur_y - 17, temp);
            cur_x += dmd.charWidth(temp);
            
            cur_x -= char_width;
            scrollX(-1);
            delay(scroll_delay);
          }
        }
        
        else if (temp == 'a' || temp == 'c'){                                   // U-kaar
          char_width = dmd.charWidth(*next);
          while((width - cur_x) <= char_width){
            dmd.drawChar(cur_x, cur_y - 5, *next);
            if (*next == '5' || *next == '@' || *next == 'K' || *next == 'Y'){
              cur_x += (dmd.charWidth(*next) - 11);
              dmd.drawChar(cur_x, cur_y + 7, temp);
              cur_x += (dmd.charWidth(temp) + 3);
            }
            else{
              cur_x += (dmd.charWidth(*next) - 8);
              dmd.drawChar(cur_x, cur_y + 7, temp);
              cur_x += dmd.charWidth(temp);
            }
            
            cur_x -= char_width;
            scrollX(-1);
            delay(scroll_delay);
          }
        }
        
        else if (temp =='b'){                                                    // UU-kaar
          char_width = dmd.charWidth(*next);
          while((width - cur_x) <= char_width){
            dmd.drawChar(cur_x, cur_y - 5, *next);
            if (*next == '5' || *next == '@' || *next == 'K' || *next == 'Y'){
              cur_x += (dmd.charWidth(*next) - 8);
              dmd.drawChar(cur_x, cur_y + 7, temp);
              cur_x += (dmd.charWidth(temp));
            }
            else{
              cur_x += (dmd.charWidth(*next) - 7);
              dmd.drawChar(cur_x, cur_y + 7, temp);
              cur_x += (dmd.charWidth(temp) - 1);
            }
            
            cur_x -= char_width;
            scrollX(-1);
            delay(scroll_delay);
          }
        }

        else if (temp == 'm'){                                                    // Khutta kateko (half letter)
          if (*(next+2) != '}' && *(next+2) >= '5' && *(next+2) <= '['){                // if followed by another character
            int cur_var = 0;

            if(*(next + 2) == 'P' && (*next == '5' || *next == 'L' || *next == 'M' || *next == 'J')){                 // if followed by Ra
              if (*next == '5'){
                *(next + 2) = 'S';
              } else if (*next == 'J'){
                *(next + 2) = 'T';
              } else if (*next == 'L'){
                *(next + 2) = 'Q';
              } else if (*next == 'M'){
                *(next + 2) = 'I';
              }
              next++;
              length--;
              goto skip;
            } else if((*next == '5' && *(next + 2) == 'W') || (*next == 'D' &&  *(next + 2) == 'P') || (*next == '<' && *(next + 2) == '>')){             // if kshya, tra and gya
              if (*next == '5'){
                *(next + 2) = 'Z';
              } else if (*next == 'D'){
                *(next + 2) = '[';
              } else if (*next == '<'){
                *(next + 2) = 'r';
              }
              next++;
              length--;
              goto skip;
            } else if(*next == 'K' || *next == '5' || *next == 'N' || *next == '<'){
              cur_var = - 2;
              shiftx = 1;
              add_width = 1;
            } else if(*next == 'V') {
              cur_var = - 4;
              shiftx = -1;
              add_width = 0;
            } else if(*next == 'P') {
              cur_var = 0;
              shiftx = 0;
              r_flag = true;
              next++;
              length--;
              goto skip;
            } else{ 
              cur_var = - 3;
              shiftx = 0;
              add_width = 0;
            }

            if (*(next+3) == '_') {
              char_width = dmd.charWidth(*next) + cur_var + dmd.charWidth('^') + dmd.charWidth(*(next + 2)) + add_width;
              while((width - cur_x) <= char_width){
                dmd.drawChar(cur_x, cur_y - 5, '^');
                cur_x += dmd.charWidth('^');
                dmd.drawChar(cur_x, cur_y - 5, *next);
                cur_x += dmd.charWidth(*next) + cur_var;
                dmd.drawChar(cur_x, cur_y - 5, '#');
                cur_x += dmd.charWidth('#') - 2;
                dmd.drawChar(cur_x, cur_y - 5, *(next+2));
                cur_x -= (shiftx + 7);
                dmd.drawChar(cur_x, cur_y - 17, ']');
                cur_x += (dmd.charWidth(']') + 4 + shiftx);
                
                cur_x -= char_width;
                scrollX(-1);
                delay(scroll_delay);
              }
              cur_x = width-1;
              dmd.drawChar(cur_x, cur_y - 5, '#');
              scrollX(-1);
              delay(scroll_delay);

              next += 2;
              length -= 2;
            }
            else {                                                                      // if no I-kaar
              char_width = dmd.charWidth(*next) + cur_var;
              while((width - cur_x) <= char_width){
                dmd.drawChar(cur_x, cur_y - 5, *next);
                scrollX(-1);
                delay(scroll_delay);
              }
              cur_x = width-1;
              dmd.drawChar(cur_x, cur_y - 5, '#');
              scrollX(-1);
              delay(scroll_delay);
            }
          }
          else{                                                                       // if not followed by another character
            char_width = dmd.charWidth(*next);
            while((width - cur_x) <= char_width){
              dmd.drawChar(cur_x, cur_y - 5, *next);
              if (*next == '5' || *next == '@' || *next == 'K' || *next == 'Y'){
                cur_x += (dmd.charWidth(*next) - 8);
                dmd.drawChar(cur_x, cur_y + 7, temp);
                cur_x += (dmd.charWidth(temp));
              }
              else{
                cur_x += (dmd.charWidth(*next) - 7);
                dmd.drawChar(cur_x, cur_y + 7, temp);
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
      else{                                                                            
        char_width = dmd.charWidth(*next);
        while((width - cur_x) <= char_width){
          dmd.drawChar(cur_x, cur_y - 5, *next);
          scrollX(-1);
          delay(scroll_delay);
        }
      }
    }
    skip:

    next++;
    length--;

    // If half R appears ------------- --------------------------------------------------------------
    if(r_flag == true) {
      r_count++;
    }

    if(r_count > 1) {
      cur_x = width-1;
      char_width = 1;
      while((width - cur_x) <= char_width){
        dmd.drawChar(cur_x - 2, cur_y - 17, 'd');
        dmd.drawChar(cur_x , cur_y - 5 , 'e');
        cur_x++;

        cur_x -= char_width;
        scrollX(-1);
        delay(scroll_delay);
      }
      r_flag = false;
      r_count = 0;
    }
  }
  for(int i = 0 ; i <= (width - cur_x) ; i++){
    scrollX(-1);
    delay(scroll_delay);
  }
}

//******************************************************************************************************************

void DMD_Nepali::scrollX(int scrollBy) {
  if(abs(scrollBy) >= width) {                                                      // scrolling over the whole display!
                                                                                    // scrolling will erase everything
    dmd.drawFilledBox(left, top, left+width-1, top+height-1, inverted ? GRAPHICS_ON : GRAPHICS_OFF);
  }
  else if(scrollBy < 0) {                                                           // Scroll left
    dmd.movePixels(left-scrollBy, top, left, top, width + scrollBy, height);
  }
  else {                                                                            // Scroll right
    dmd.movePixels(left, top, left+scrollBy, top, width - scrollBy, height);
  }

  cur_x += scrollBy;
  while(cur_x < 0)
    cur_x += width;
  while(cur_x > width)
    cur_x -= width;
}

//******************************************************************************************************************

void DMD_Nepali::clear() {
  this->reset();
  dmd.drawFilledBox(left,top,left+width,top+height,inverted ? GRAPHICS_ON : GRAPHICS_OFF);
}

//******************************************************************************************************************

void DMD_Nepali::reset() {
  cur_x = 0;
  cur_y = 0;
  pending_newline = false;
}