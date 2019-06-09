//#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
//#include <String.h>
#include <WiFiClient.h>

bool setIPFlag = false;
bool configModeFlag = false;

const char *ssidAP = "sanPBL";
const char *passwordAP = "SANPUBLICATION123";

ESP8266WebServer server(80);

void SETUP_WIFI()
{
    WiFi.softAP(ssidAP, passwordAP);
    delay(500);
    //  Serial.print ( "IP address: " );
    //  Serial.println (WiFi.softAPIP());
}

void StartServer()
{
    // Handle Routes
    server.on("/", handleRoot);
    server.on("/sanjiwani", handleQueryArgs);
    server.onNotFound(handleNotFound);

    server.begin();
    //  Serial.println("Server Started");
}

void handleRoot()
{
    server.send(200, "text/plain", "hello from server");
}

void handleQueryArgs()
{ 
    SendThruSerial();
    server.send(200, "text/plain", "i heard you");
}

void handleNotFound()
{
    server.send(404, "text/plain", "Error 404! Incorrect Path");
}

void SendThruSerial()
{
    String serialMsg;
    if(server.argName(0)){
        serialMsg = server.argName(0) + "->" + server.arg(0) + "|";
        Serial.println(serialMsg);
        Serial.flush();
    }
}

/* ********** END - UTILS **************** */

void RunBootupTasks()
{
    SETUP_WIFI();
    StartServer();
}

/***************************************************************/

void setup()
{
    Serial.begin(9600);
    //  SETUP_EEPROM();

    delay(100);

    RunBootupTasks();

    delay(100);
    //  Serial.println("Bootup Complete");
    delay(1000);
}

void loop()
{
    server.handleClient();
}
