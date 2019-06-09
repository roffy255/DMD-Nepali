// #ifndef my_animation
// #define my_animation

// #if (ARDUINO >= 100)
// #include "Arduino.h"
// #else
// #include "WProgram.h"
// #endif

// #include <DMD2.h>
// #include <DMD_Nepali.h>

// class animation
// {

// public:
//   animation(DMDFrame &dmdFrame, SoftDMD &dmd, int width, int height);

//   void snakeLeft(int x, int call);
//   void snakeRight(int x, int call);
//   void fadingEffect();
//   void cleaningCurtain();
//   void drawLine(int x1, int y1, int x2, int y2);
//   void curtainVertical();
//   void plainBoarder();
//   void curtainHorizantalLeft();
//   void curtainHorizantalRight();
//   void helmet_empty(int x, int y);
//   void helmet_emptyLeft(int x, int y);
//   void helmet_emptyRight(int x, int y);
//   void cup(int x, int y);
//   void cup_vapour2(int x, int y, int x1, int y1, int delay_cup_empty_vapour);
//   void Cup_Empty_Vapour(int x, int y, int delay_cup_empty_vapour);
//   void Smiley(int x, int y);
//   void Love(int x, int y);
//   void Thumbs_Up(int x, int y);
//   void Wink(int x, int y);
//   void Sad(int x, int y);
//   void Sun(int x, int y);
//   void Danger_Sign(int x, int y);
//   void Arrow(int x, int y);
//   void Stop_Sign(int x, int y);
//   void Man(int x, int y);
//   void Man_2(int x, int y);
//   void SUN(int x, int y);
//   void FULL_SUN(int x, int y);
//   void ASUN_2(int x, int y);
//   void sun_3(int x, int y, int delayVal);
//   void sun_2(int x, int y);
//   void star(int x, int y);
//   void moon(int x, int y);
//   void DIAGONAL_moon(int x, int y);
//   void Vertical_moon(int x, int y);
//   void women(int x, int y);
//   void women_2(int x, int y);
//   void swastikSign(int x, int y);
//   void Vass(int x, int y);
//   void Ganesh(int x, int y);
//   void Crown(int x, int y);
//   void sanjiwaniLogo(int x, int y);
//   void operaHouse(int x, int y);
//   void statueOfLiberty(int x, int y);
//   void Lotus(int x, int y);
//   void cnTower(int x, int y);
//   void cnTower1(int x, int y);
//   void skyTower(int x, int y);
//   void Lotus0(int x, int y);
//   void operaHouse1(int x, int y);
//   void londonBridge(int x, int y);
//   void operaHouse1_off(int x, int y);
//   void londonBridge_off(int x, int y);
//   void canada(int x, int y);
//   void aeroplane(int x, int y);
//   void cloud(int x, int y);
//   void flagStill(int x, int y);
//   void flagShift(int x, int y);
//   void flagRoll(int x, int y);
//   void sanBook1(int x, int y);
//   void sanBook2(int x, int y);
//   void sanlogo(int x, int y);
//   void santext(int x, int y);
//   void table(int x, int y);
//   void dhakal(int x , int y);
//   void scroll(int x, int y);
//   void hotelhalesi(int x,  int y);
//   void dateSeperator(int x, int y);
//   void timeSeperator(int x, int y);
//   void divider(int x, int y);
//   void timeSeperatorCompliment(int x, int y);
//   void degreeC(int x,  int y);
//   void sanjiwani(int x, int y);
//   void nilkantheswor(int x, int y);
//   void petrol(int x, int y);
//   void diesel(int x, int y);
//   void ramechap1(int x, int y);

// private:
//   // DMDFrame &dmdFrame;
//   // SoftDMD &dmd;
//   DMDFrame &dmd;
//   SoftDMD &dmdbright;
//   int width;
//   int height;
//   void delay_engine(int delay_val);

//   void stLineDiagP(int x, int y, int lengthLine);
//   void stLineDiagN(int x, int y, int lengthLine);
//   void stLineHor(int x, int y, int lengthLine);
//   void stLineVer(int x, int y, int lengthLine);
// };

// #endif

#ifndef my_animation
#define my_animation

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <DMD2.h>
#include <DMD_Nepali.h>

class animation
{

public:
  animation(DMDFrame &dmdFrame, SPIDMD &dmd, int width, int height);

  void snakeLeft(int x, int call);
  void snakeRight(int x, int call);
  void fadingEffect();
  void cleaningCurtain();
  void drawLine(int x1, int y1, int x2, int y2);
  void curtainVertical();
  void plainBoarder();
  void curtainHorizantalLeft();
  void curtainHorizantalRight();
  void helmet_empty(int x, int y);
  void helmet_emptyLeft(int x, int y);
  void helmet_emptyRight(int x, int y);
  void cup(int x, int y);
  void cup_vapour2(int x, int y, int x1, int y1, int delay_cup_empty_vapour);
  void Cup_Empty_Vapour(int x, int y, int delay_cup_empty_vapour);
  void Smiley(int x, int y);
  void Love(int x, int y);
  void Thumbs_Up(int x, int y);
  void Wink(int x, int y);
  void Sad(int x, int y);
  void Sun(int x, int y);
  void Danger_Sign(int x, int y);
  void Arrow(int x, int y);
  void Stop_Sign(int x, int y);
  void Man(int x, int y);
  void Man_2(int x, int y);
  void SUN(int x, int y);
  void FULL_SUN(int x, int y);
  void ASUN_2(int x, int y);
  void sun_3(int x, int y, int delayVal);
  void sun_2(int x, int y);
  void star(int x, int y);
  void moon(int x, int y);
  void DIAGONAL_moon(int x, int y);
  void Vertical_moon(int x, int y);
  void women(int x, int y);
  void women_2(int x, int y);
  void swastikSign(int x, int y);
  void Vass(int x, int y);
  void Ganesh(int x, int y);
  void Crown(int x, int y);
  void sanjiwaniLogo(int x, int y);
  void operaHouse(int x, int y);
  void statueOfLiberty(int x, int y);
  void Lotus(int x, int y);
  void cnTower(int x, int y);
  void cnTower1(int x, int y);
  void skyTower(int x, int y);
  void Lotus0(int x, int y);
  void operaHouse1(int x, int y);
  void londonBridge(int x, int y);
  void operaHouse1_off(int x, int y);
  void londonBridge_off(int x, int y);
  void canada(int x, int y);
  void aeroplane(int x, int y);
  void cloud(int x, int y);
  void flagStill(int x, int y);
  void flagShift(int x, int y);
  void flagRoll(int x, int y);
  void sanBook1(int x, int y);
  void sanBook2(int x, int y);
  void sanlogo(int x, int y);
  void santext(int x, int y);
  void table(int x, int y);
  void dhakal(int x, int y);
  void scroll(int x, int y);
  void hotelhalesi(int x, int y);
  void dateSeperator(int x, int y);
  void timeSeperator(int x, int y);
  void divider(int x, int y);
  void timeSeperatorCompliment(int x, int y);
  void degreeC(int x, int y);
  void sanjiwani(int x, int y);
  void nilkantheswor(int x, int y);
  void petrol(int x, int y);
  void diesel(int x, int y);
  void ramechap1(int x, int y);
  void startailor(int x, int y);
  void palika(int x, int y);

private:
  // DMDFrame &dmdFrame;
  // SPIDMD &dmd;
  DMDFrame &dmd;
  SPIDMD &dmdbright;
  int width;
  int height;
  void delay_engine(int delay_val);

  void stLineDiagP(int x, int y, int lengthLine);
  void stLineDiagN(int x, int y, int lengthLine);
  void stLineHor(int x, int y, int lengthLine);
  void stLineVer(int x, int y, int lengthLine);
};

#endif