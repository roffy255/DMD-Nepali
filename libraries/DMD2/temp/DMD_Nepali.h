#ifndef DMD_NEPALI_H
#define DMD_NEPALI_H

#include "DMD_Nepali.h"
#include "DMD2.h"
#include "fonts/SystemFont5x7.h"
#include "fonts/NepFont1_16bit.h"

class DMD_Nepali
{
public:
	DMD_Nepali(DMDFrame &dmd, int left = 0, int top = 0, int width = 0, int height = 0);
	char* charMappingESP(char*);
  char* charMapping(char*);
  void printString(char*, uint8_t);
  void scrollString(char*, uint8_t, int);
  uint8_t getCharacterCount();

	void clear();
  void reset();
	void scrollX(int scrollBy);
	void scrollY(int scrollBy);
  void checkScroll();

	bool change_flag = 0;
  char m[200] = {0};
  int scroll_by = 0;
  bool scroll_flag = false;
  int scroll_delay = 80;
  bool r_flag = false;
  uint8_t r_count = 0;
  unsigned char char_count = 0;

  int add_width;
private:
	DMDFrame &dmd;
  	bool inverted;
 	  int left;
  	int top;
  	int width;
  	int height;
  	int16_t cur_x;
  	int16_t cur_y;
  	bool pending_newline;

    uint8_t char_width;
};

#endif