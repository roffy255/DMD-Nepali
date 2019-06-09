#include <DMD2.h>
#include <fonts/NepFontBold.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoOTA.h>

bool setIPFlag = false;
bool configModeFlag = false;
bool ack = false;

const char *ssidAP = "roomforreading";
const char *passwordAP = "CLFA5C17C0";
const char *ota_hostname = "NEPALIFONT";

ESP8266WebServer server(80);

SPIDMD dmd(1, 1);

const char alphabet[38] = {'5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

void setup()
{
    Serial.begin(9600);
     WiFi.begin(ssidAP, passwordAP);
    int timer1 = millis();
    OTA_flag = true;
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
        if (millis() - timer1 > 7000)
        {
            Serial.println("connection failed");
            OTA_flag = false;
            break;
        }
    }
    Serial.print("\nConnected to '");
    Serial.print(ssidAP);
    Serial.print("' and got IP ");
    Serial.println(WiFi.localIP());
    dmd.begin();
    dmd.selectFont(NepFont_Bold);

}

void beginOTA(const char *hostname)
{
    ArduinoOTA.setHostname(hostname);
    ArduinoOTA.onStart([]() {
        Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
            Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
            Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
            Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
            Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
            Serial.println("End Failed");
    });
    ArduinoOTA.begin();
    Serial.print("OTA is up");
}

void loop()
{
    for (int i = 0; i < 38; i++)
    {

        dmd.drawChar(0, 0, alphabet[i]);
        delay(1000);
        dmd.clearScreen();
    }
}