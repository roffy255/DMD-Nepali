//#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
//#include <String.h>
#include <WiFiClient.h>

/* *** Global Variables *** */
bool setIPFlag = false;
bool configModeFlag = false;
bool ack = false;

//Wifi
//const String startConfigMsg = "sc";
const char* ssidAP = "DBASchool";
const char* passwordAP = "DBASchool";

ESP8266WebServer server(80);

void SETUP_WIFI() {
  WiFi.softAP(ssidAP, passwordAP);
  delay(500);
}

void StartServer() {
  // Handle Routes
  server.on("/", handleRoot);
  server.on("/e4c5e78e17a0f01f0f0fa1f3d163bdd8", handleQueryArgs);
  server.onNotFound(handleNotFound);

  server.begin();
  //  Serial.println("Server Started");
}

void handleRoot() {
  server.send(200, "text/plain", "HELLO");
}

void handleQueryArgs() {   //Handler
  String response = SendThruSerial(); 
  server.send(200, "text/plain", response);
}

void handleNotFound(){  
  server.send(404, "text/plain", "Error 404! Incorrect Path");
}

String SendThruSerial() {  
  

  String serialMsg;
  String response;
  String key = server.argName(0);
  String value = server.arg(0);
    if(key == "M" & value.length() > 100){
      int val_len = value.length();
      int div;
      if(val_len % 100 == 0){
        div = (val_len / 100);
      }else{
        div = (val_len / 100) + 1;
      }
      if(val_len > 100){
        digitalWrite(0, HIGH);
         Serial.println("M->" + value.substring(0,100) + "|");
         Serial.flush();
         value = value.substring(100,value.length());
         delay(2000);
        for(int i = 1; i < div; i++){
         Serial.println("->" + value.substring(0,100) + "|");
         Serial.flush();
         delay(2000);
         value = value.substring(100,value.length());
      }
      }    
      }
      else{
        serialMsg = key + "->" + value + "|";
        Serial.println(serialMsg);
        Serial.flush();
      }
  Serial.flush();
  return response;
                                                                                                     
  }
 

void RunBootupTasks() {
  SETUP_WIFI();
  StartServer();
}

void setup() {
  Serial.begin(9600);
  delay(100);
  
  RunBootupTasks();

  delay(100);

  delay(1000);
}

void loop() {
  server.handleClient();
}
