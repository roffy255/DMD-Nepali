#ifndef DMD_NEPALI_H
#define DMD_NEPALI_H

#include "DMD_Nepali.h"
#include "DMD2.h"
#include "fonts/SystemFont5x7.h"
#include "fonts/Arial14.h"
#include "fonts/NepFont1_16bit.h"
#include "fonts/NepFontBold.h"

class DMD_Nepali
{
public:
	DMD_Nepali(DMDFrame &dmd, int left = 0, int top = 0, int width = 0, int height = 0);
	char* charMappingESP(char*);
  char* charMapping(char*);
  char* charMappingFromInt(char*, char);
  char* parseCsv(char*, char);
  void drawString(int, int, String);
  void printString(char*, uint8_t);
  void scrollString(char*, uint8_t);
  void drawBoldString(int, int, String);
  void scrollBoldString(int, int, String);
  uint8_t getCharacterCount();

	void clear();
  void reset();
	void scrollX(int scrollBy);
	// void scrollY(int scrollBy);
  void movePixels(unsigned int from_x, unsigned int from_y,
                         unsigned int to_x, unsigned int to_y,
                         unsigned int width, unsigned int height);
  void checkScroll();

	bool change_flag = 0;
  char m[200] = {0};
  int scroll_by = 0;
  bool scroll_flag = false;
  int scroll_delay = 80;
  bool r_flag = false;
  uint8_t r_count = 0;
  uint8_t char_count = 0;
  int add_width;

  const uint8_t *font_nepali = NepFont1_16bit;
  const uint8_t *font_eng = SystemFont5x7;
  uint8_t eng_y_offset = 5;
  
private:
	DMDFrame &dmd;
  //DMDFrame &to_move;
  	bool inverted;
 	  int left;
  	int top;
  	int width;
  	int height;
  	int16_t cur_x;
  	int16_t cur_y;
  	bool pending_newline;

    uint8_t char_width;	

  void drawString(int, int, char*, uint8_t);
  void drawBoldString(int, int, char*, uint8_t);
  void scrollBoldString(int, int, char*, uint8_t);
};

#endif