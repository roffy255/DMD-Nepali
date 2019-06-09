#include <ArduinoOTA.h>

const char *ssid = "clownshoes";
const char *password = "clownshoes";
const char *ota_hostname = "clownshoes";
IPAddress apIP(192, 168, 1, 1);

// starts an access point
void beginAP(const char *ssid, const char *password, IPAddress ip)
{
    //WiFi.mode(WIFI_AP);
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(ip, ip, IPAddress(255, 255, 255, 0));
    WiFi.softAP(ssid, password);
    Serial.print("AP is up at ");
    Serial.println(WiFi.softAPIP());
}

// connects to an access point
void connectToAP(const char *ssid, const char *password)
{
    Serial.print("");
    WiFi.begin(ssid, password);
    int timer1 = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
        if (millis() - timer1 > 5000)
        {
            Serial.println("connection failed");
            break;
        }
    }
    Serial.print("\nConnected to '");
    Serial.print(ssid);
    Serial.print("' and got IP ");
    Serial.println(WiFi.localIP());
}

// starts OTA server
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

void setup()
{
    Serial.begin(9600);
    connectToAP(ssid, password);
    beginAP(ssid, password, apIP);
    delay(500);
    beginOTA(ota_hostname);
}

void loop()
{
    ArduinoOTA.handle();
}