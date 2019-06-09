#include <DMD2.h>
#include <DMD_Nepali.h>
#include <fonts/NepFont1_16bit.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

SoftDMD dmd(1, 1);
DMD_Nepali nep(dmd);
float Celsius = 0;
float Fahrenheit = 0;

void setup()
{
    sensors.begin();
    Serial.begin(9600);
    dmd.begin();
    dmd.setBrightness(255);
}

void gettemperature()
{
    sensors.requestTemperatures();
    Celsius = sensors.getTempCByIndex(0);
    Serial.println(Celsius);
    Fahrenheit = sensors.toFahrenheit(Celsius);
}

String decode_date(String eng_date)
{
    uint8_t date_len = eng_date.length();
    String nep_date = "";
    for (uint8_t i = 0; i < date_len; i++)
    {
        if (eng_date[i] == '0')
        {
            nep_date = nep_date + "s";
        }
        else if (eng_date[i] == '1')
        {
            nep_date = nep_date + "t";
        }
        else if (eng_date[i] == '2')
        {
            nep_date = nep_date + "u";
        }
        else if (eng_date[i] == '3')
        {
            nep_date = nep_date + "v";
        }
        else if (eng_date[i] == '4')
        {
            nep_date = nep_date + "w";
        }
        else if (eng_date[i] == '5')
        {
            nep_date = nep_date + "x";
        }
        else if (eng_date[i] == '6')
        {
            nep_date = nep_date + "y";
        }
        else if (eng_date[i] == '7')
        {
            nep_date = nep_date + "z";
        }
        else if (eng_date[i] == '8')
        {
            nep_date = nep_date + "{";
        }
        else if (eng_date[i] == '9')
        {
            nep_date = nep_date + "|";
        }
    }
    return nep_date;
}

void processData()
{
    gettemperature();
    int temp = Celsius;
    String temperature = decode_date(String(temp,DEC));
    Serial.println(temperature);
    displayTemp(temperature);
}

void displayTemp(String temp_val)
{
    dmd.selectFont(NepFont1_16bit);
    nep.drawString(0, 0, "~" + temp_val + '$');
}
void loop()
{
    processData();
    delay(5000);
}