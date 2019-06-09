//#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
//#include <String.h>
#include <WiFiClient.h>

/* *** Global Variables *** */\
bool setIPFlag = false;
bool configModeFlag = false;

//Wifi
//const String startConfigMsg = "sc";
const char* ssidAP = "BHJewellers";
const char* passwordAP = "BHJewellers123";

//EEPROM
//const int eepromSize = 512;

/* ******** FOR EEPROM ********* */

//void SETUP_EEPROM() {
//  EEPROM.begin(eepromSize);
//}

//void ClearEEPROM(){
//  for(int i = 0 ; i < eepromSize ; i++) {
//    EEPROM.write(i, 0); 
//  }
//  EEPROM.commit();
//}

/* ******** END EEPROM ********* */

/* ********** For WIFI *********** */

ESP8266WebServer server(80);

void SETUP_WIFI() {
  WiFi.softAP(ssidAP, passwordAP);
  delay(500);
  //  Serial.print ( "IP address: " );
  //  Serial.println (WiFi.softAPIP());
}

void StartServer() {
  // Handle Routes
  server.on("/", handleRoot);
  server.on("/8967c1270f826f411d9630b5e845d755", handleQueryArgs);
  server.onNotFound(handleNotFound);

  server.begin();
  //  Serial.println("Server Started");
}

/* ********** END - WIFI *********** */

/* ********** Request Handlers *********** */

void handleRoot() {

  server.send(200, "text/plain", "hello" );
}

void handleQueryArgs() {   //Handler
  String response = SendThruSerial(); 
  server.send(200, "text/plain", response);
}

void handleNotFound(){  
  server.send(404, "text/plain", "Error 404! Incorrect Path");
}

/* ********* END- HANDLERS *************** */

/* ************ Util Functions ************* */

String SendThruSerial() {  
  //format(priceUpdate) : price_tGold->[price]~price_fGold->[price]~price_silver->[price]
  //format(datetime)    : d->[ccyymmddHHMMSS] 

  String serialMsg;
  String response;
  
  for (int i = 0; i < server.args(); i++) {
    String key = server.argName(i);
    String value = server.arg(i);
    serialMsg += key + "->" + value + "~";
    response += "Set: " + key + "=" + value + "\n";                                                                                                     
  }
  response += serialMsg;
  Serial.println(serialMsg);
  Serial.flush();
  return response;
}

/* ********** END - UTILS **************** */

void RunBootupTasks() {
  SETUP_WIFI();
  StartServer();
}

/***************************************************************/

void setup() {
  Serial.begin(4800);
//  SETUP_EEPROM();
  
  delay(100);
  
  RunBootupTasks();

  delay(100);
 //  Serial.println("Bootup Complete");
  delay(1000);
}

void loop() {
  server.handleClient();
}
