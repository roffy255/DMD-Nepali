
#include <PxMatrix.h>
#include <EEPROM.h>
#include <DS3231.h>
#include <Wire.h>
#include <SPI.h>

const char *ssid = "RGB";
const char *password = "redbluegreen";


#ifdef ESP8266

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

ESP8266WebServer server(80);

#endif


#ifdef ESP32

#define P_LAT 22
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15
#define P_OE 2
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

#endif

#ifdef ESP8266

#include <Ticker.h>
Ticker display_ticker;
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2

#endif
// Pins for LED MATRIX

#define matrix_width 32
#define matrix_height 16

uint8_t display_draw_time = 0;

PxMATRIX display(matrix_width, matrix_height, P_LAT, P_OE, P_A, P_B, P_C);
//PxMATRIX display(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D);
//PxMATRIX display(64,64,P_LAT, P_OE,P_A,P_B,P_C,P_D,P_E);

// Some standard colors
uint16_t myRED = display.color565(255, 0, 0);
uint16_t myGREEN = display.color565(0, 255, 0);
uint16_t myBLUE = display.color565(0, 0, 255);
uint16_t myWHITE = display.color565(255, 255, 255);
uint16_t myYELLOW = display.color565(255, 255, 0);
uint16_t myCYAN = display.color565(0, 255, 255);
uint16_t myMAGENTA = display.color565(255, 0, 255);
uint16_t myBLACK = display.color565(0, 0, 0);

uint16_t myCOLORS[8] = {myRED, myGREEN, myBLUE, myWHITE, myYELLOW, myCYAN, myMAGENTA, myBLACK};

#ifdef ESP8266
// ISR for display refresh
void display_updater()
{
    display.display(display_draw_time);
}
#endif

#ifdef ESP32
void IRAM_ATTR display_updater()
{
    // Increment the counter and set the time of ISR
    portENTER_CRITICAL_ISR(&timerMux);
    display.display(display_draw_time);
    portEXIT_CRITICAL_ISR(&timerMux);
}
#endif

void display_update_enable(bool is_enable)
{

#ifdef ESP8266
    if (is_enable)
        display_ticker.attach(0.002, display_updater);
    else
        display_ticker.detach();
#endif

#ifdef ESP32
    if (is_enable)
    {
        timer = timerBegin(0, 80, true);
        timerAttachInterrupt(timer, &display_updater, true);
        timerAlarmWrite(timer, 2000, true);
        timerAlarmEnable(timer);
    }
    else
    {
        timerDetachInterrupt(timer);
        timerAlarmDisable(timer);
    }
#endif
}



//const String color_code = "fea0bb027689ff0000";
const String color_code = "ffffff000000ff0000";
const String image = "16A16A16A16A16A16A16A5A7B4A3A3B3A2C2B3A2A2B5A2C4B1A1A2B2C3A2C2B2A2B1A1B2A6C1B4A1B2B2A2C3A1C1B4A1B1B2A2C5A3B2A1B1B4C5A1B4A1B1B4C5A1B4A1B1B2A2C5A3B2A1B2B2A2C3A1C1B4A1B1A1B2A6C1B4A1B1A2B2C3A2C2B2A2B2A2B5A2C4B1A3A3B3A2C2B3A5A7B4A16A16A16A16A16A16A16A16A16A";
uint8_t color_int_code[30] = {};
String image_code = image;
char colorId;
uint8_t red_val = 0;
uint8_t blue_val = 0;
uint8_t green_val = 0;

void decode_color()
{
    String sub_color = color_code;
    char color_element;
    uint8_t hex_int1;
    uint8_t hex_int2;
    uint8_t color_factor = sub_color.length() / 6;
//    Serial.println(color_factor);
    bool alternator = true;
    int count = 0;

    for (int8_t j = 0; j < sub_color.length(); j++)
    {
        color_element = sub_color[j];
//        Serial.println(color_element);
        if (alternator == true)
        {

            if (color_element == 'a' || color_element == 'b' || color_element == 'c' || color_element == 'd' || color_element == 'e' || color_element == 'f')
            {

                hex_int1 = int(color_element) - 87;
            }
            else
            {

                hex_int1 = int(color_element) - 48;
            }
            alternator = false;
        }
        else
        {
            if (color_element == 'a' || color_element == 'b' || color_element == 'c' || color_element == 'd' || color_element == 'e' || color_element == 'f')
            {

                hex_int2 = int(color_element) - 87;
            }
            else
            {

                hex_int2 = int(color_element) - 48;
            }
            color_int_code[count] = (hex_int1 << 4) | hex_int2;
//            Serial.println(color_int_code[count], DEC);
            count++;
            alternator = true;
        }
    }
}

uint8_t analyzeQuantity()
{
    String extend = "";
    int extendNumber;
    uint8_t i = 0;
    char element = image_code[i];
    int element_ascii = int(element);
    while (element_ascii < 65 && element_ascii > 47)
    {
        extend = String(extend + element);
        i++;
        element = image_code[i];
        element_ascii = int(element);
    }
    image_code = image_code.substring(i+1, image_code.length());
    extendNumber = extend.toInt();
  
//    Serial.println(extendNumber);
    colorId = element;
  
//    Serial.println(colorId);
//    Serial.println(image_code);
    return extendNumber;
}

void parseColor(){
    uint8_t actual_color = (int(colorId) - 65)*3;
    red_val = color_int_code[actual_color];
    green_val = color_int_code[actual_color + 1];
    blue_val = color_int_code[actual_color + 2];
}

void display_image()
{
    decode_color();
    int image_size = image_code.length();
    uint8_t quantity_count = 0;
    for (int i = 0; i < 1 * 32; i++)
    {
        for (int j = 0; j < 1 * 16; j++)
        {
            if (quantity_count <= 0){
                quantity_count = analyzeQuantity();
            }
            if (quantity_count > 0){
                parseColor();
                display.drawPixel(i, j, display.color565(red_val, blue_val, green_val));
            }
            quantity_count--;
        }
    }
    image_code = image;
}

void setup()
{
    Serial.begin(9600);
    // Define your display layout here, e.g. 1/8 step
    display.begin(8);

    #ifdef ESP8266
    WiFi.softAP(ssid, password);
    delay(500);
//    server.on("/", handleRoot);
//    server.on("/M", handleQueryArgs);
//    server.onNotFound(handleNotFound);
    server.begin();
    delay(500);
    #endif


    // Define your scan pattern here {LINE, ZIGZAG, ZAGGIZ} (default is LINE)
    //display.setScanPattern(LINE);

    // Define multiplex implemention here {BINARY, STRAIGHT} (default is BINARY)
    //display.setMuxPattern(BINARY);

    // Compare draw latency at similar display brightness for standard and fast drawing
    display.setFastUpdate(false);
    Serial.println("Draw test without fast update");
    display_draw_time = 15;

    Serial.println("Draw test with fast update");
    display.setFastUpdate(true);
    display_draw_time = 0;

    display.setFastUpdate(false);
    display_draw_time = 15;
    display.clearDisplay();
    display_update_enable(true);
    //display.setColorOffset(15,15,0);
}
union single_double {
    uint8_t two[2];
    uint16_t one;
} this_single_double;

// This draws the weather icons

unsigned long last_draw = 0;

int count, count2;
void loop()
{

    display_image();
    server.handleClient();

}
