#include <DS3231.h>
#include <Wire.h>

DS3231 Clock;
bool h12;
bool PM;

void setup() 
{
  Wire.begin();
  Serial.begin(9600);
//   Clock.setHour(1);
//  Clock.setMinute(20);
//  Clock.setSecond(30);

}

void loop() 
{

  
 Serial.print(Clock.getHour(h12, PM), DEC);
 Serial.print(" :");
 Serial.print(Clock.getMinute(), DEC);
 Serial.print(" :");
 Serial.println(Clock.getSecond(), DEC);
 delay(1000);

}
