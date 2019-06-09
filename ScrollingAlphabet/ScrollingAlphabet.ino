/*
  Scrolling alphabet demo, displays characters one at a time into a scrolling box.
 */

#include <SPI.h>
#include <DMD2.h>
#include <DMD_Nepali.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/Arial14.h>
#include <fonts/Droid_Sans_24.h>

// Set Width to the number of displays wide you have
const int WIDTH = 1;

// You can change to a smaller font (two lines) by commenting this line,
// and uncommenting the line after it:
//const uint8_t *FONT = Arial14;
const uint8_t *FONT = SystemFont5x7;

String MESSAGE = "ROOM FOR READING ";

SPIDMD dmd(WIDTH,1);  // DMD controls the entire display
DMD_TextBox box(dmd);  // "box" provides a text box to automatically write to/scroll the display
DMD_Nepali nep(dmd);

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
  dmd.setBrightness(255);
  dmd.selectFont(FONT);
  dmd.begin();
}

// the loop routine runs over and over again forever:
void loop() {
  char next[MESSAGE.length()] ;
  MESSAGE.toCharArray(next, MESSAGE.length());
//  nep.scrollString(next, MESSAGE.length(), 5);

  for (int i = 64; i == -MESSAGE.length()*5; i--){
    dmd.drawString(i, 5, next);
    delay(500);
    dmd.clearScreen();
  }
}
