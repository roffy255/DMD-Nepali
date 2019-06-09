//#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
//#include <String.h>
#include <WiFiClient.h>


bool setIPFlag = false;
bool configModeFlag = false;

const char* ssidAP = "MMJewellers";
const char* passwordAP = "MMJewellers123";

ESP8266WebServer server(80);
const char html_hr[] PROGMEM = {"<!DOCTYPE html>\n<html>\n<title>W3.CSS Template</title>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n<link rel=\"stylesheet\" href=\"https://www.w3schools.com/w3css/4/w3.css\">\n<link rel='stylesheet' href='https://fonts.googleapis.com/css?family=Roboto'>\n<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css\">\n<style>\nhtml,body,h1,h2,h3,h4,h5,h6 {font-family: \"Roboto\", sans-serif}\n</style>\n<body class=\"w3-light-grey\">\n\n<!-- Page Container -->\n<div class=\"w3-content w3-margin-top\" style=\"max-width:1400px;\">\n\n  <!-- The Grid -->\n  <div class=\"w3-row-padding\">\n  \n    <!-- Left Column -->\n    <div class=\"w3-third\">\n    \n      <div class=\"w3-white w3-text-grey w3-card-4\">\n        <div class=\"w3-display-container\">\n          <img src=\"/w3images/avatar_hat.jpg\" style=\"width:100%\" alt=\"Avatar\">\n          <div class=\"w3-display-bottomleft w3-container w3-text-black\">\n            <h2>Jane Doe</h2>\n          </div>\n        </div>\n        <div class=\"w3-container\">\n          <p><i class=\"fa fa-briefcase fa-fw w3-margin-right w3-large w3-text-teal\"></i>Designer</p>\n          <p><i class=\"fa fa-home fa-fw w3-margin-right w3-large w3-text-teal\"></i>London, UK</p>\n          <p><i class=\"fa fa-envelope fa-fw w3-margin-right w3-large w3-text-teal\"></i>ex@mail.com</p>\n          <p><i class=\"fa fa-phone fa-fw w3-margin-right w3-large w3-text-teal\"></i>1224435534</p>\n          <hr>\n\n          <p class=\"w3-large\"><b><i class=\"fa fa-asterisk fa-fw w3-margin-right w3-text-teal\"></i>Skills</b></p>\n          <p>Adobe Photoshop</p>\n          <div class=\"w3-light-grey w3-round-xlarge w3-small\">\n            <div class=\"w3-container w3-center w3-round-xlarge w3-teal\" style=\"width:90%\">90%</div>\n          </div>\n          <p>Photography</p>\n          <div class=\"w3-light-grey w3-round-xlarge w3-small\">\n            <div class=\"w3-container w3-center w3-round-xlarge w3-teal\" style=\"width:80%\">\n              <div class=\"w3-center w3-text-white\">80%</div>\n            </div>\n          </div>\n          <p>Illustrator</p>\n          <div class=\"w3-light-grey w3-round-xlarge w3-small\">\n            <div class=\"w3-container w3-center w3-round-xlarge w3-teal\" style=\"width:75%\">75%</div>\n          </div>\n          <p>Media</p>\n          <div class=\"w3-light-grey w3-round-xlarge w3-small\">\n            <div class=\"w3-container w3-center w3-round-xlarge w3-teal\" style=\"width:50%\">50%</div>\n          </div>\n          <br>\n\n          <p class=\"w3-large w3-text-theme\"><b><i class=\"fa fa-globe fa-fw w3-margin-right w3-text-teal\"></i>Languages</b></p>\n          <p>English</p>\n          <div class=\"w3-light-grey w3-round-xlarge\">\n            <div class=\"w3-round-xlarge w3-teal\" style=\"height:24px;width:100%\"></div>\n          </div>\n          <p>Spanish</p>\n          <div class=\"w3-light-grey w3-round-xlarge\">\n            <div class=\"w3-round-xlarge w3-teal\" style=\"height:24px;width:55%\"></div>\n          </div>\n          <p>German</p>\n          <div class=\"w3-light-grey w3-round-xlarge\">\n            <div class=\"w3-round-xlarge w3-teal\" style=\"height:24px;width:25%\"></div>\n          </div>\n          <br>\n        </div>\n      </div><br>\n\n    <!-- End Left Column -->\n    </div>\n\n    <!-- Right Column -->\n    <div class=\"w3-twothird\">\n    \n      <div class=\"w3-container w3-card w3-white w3-margin-bottom\">\n        <h2 class=\"w3-text-grey w3-padding-16\"><i class=\"fa fa-suitcase fa-fw w3-margin-right w3-xxlarge w3-text-teal\"></i>Work Experience</h2>\n        <div class=\"w3-container\">\n          <h5 class=\"w3-opacity\"><b>Front End Developer / w3schools.com</b></h5>\n          <h6 class=\"w3-text-teal\"><i class=\"fa fa-calendar fa-fw w3-margin-right\"></i>Jan 2015 - <span class=\"w3-tag w3-teal w3-round\">Current</span></h6>\n          <p>Lorem ipsum dolor sit amet. Praesentium magnam consectetur vel in deserunt aspernatur est reprehenderit sunt hic. Nulla tempora soluta ea et odio, unde doloremque repellendus iure, iste.</p>\n          <hr>\n        </div>\n        <div class=\"w3-container\">\n          <h5 class=\"w3-opacity\"><b>Web Developer / something.com</b></h5>\n          <h6 class=\"w3-text-teal\"><i class=\"fa fa-calendar fa-fw w3-margin-right\"></i>Mar 2012 - Dec 2014</h6>\n          <p>Consectetur adipisicing elit. Praesentium magnam consectetur vel in deserunt aspernatur est reprehenderit sunt hic. Nulla tempora soluta ea et odio, unde doloremque repellendus iure, iste.</p>\n          <hr>\n        </div>\n        <div class=\"w3-container\">\n          <h5 class=\"w3-opacity\"><b>Graphic Designer / designsomething.com</b></h5>\n          <h6 class=\"w3-text-teal\"><i class=\"fa fa-calendar fa-fw w3-margin-right\"></i>Jun 2010 - Mar 2012</h6>\n          <p>Lorem ipsum dolor sit amet, consectetur adipisicing elit. </p><br>\n        </div>\n      </div>\n\n      <div class=\"w3-container w3-card w3-white\">\n        <h2 class=\"w3-text-grey w3-padding-16\"><i class=\"fa fa-certificate fa-fw w3-margin-right w3-xxlarge w3-text-teal\"></i>Education</h2>\n        <div class=\"w3-container\">\n          <h5 class=\"w3-opacity\"><b>W3Schools.com</b></h5>\n          <h6 class=\"w3-text-teal\"><i class=\"fa fa-calendar fa-fw w3-margin-right\"></i>Forever</h6>\n          <p>Web Development! All I need to know in one place</p>\n          <hr>\n        </div>\n        <div class=\"w3-container\">\n          <h5 class=\"w3-opacity\"><b>London Business School</b></h5>\n          <h6 class=\"w3-text-teal\"><i class=\"fa fa-calendar fa-fw w3-margin-right\"></i>2013 - 2015</h6>\n          <p>Master Degree</p>\n          <hr>\n        </div>\n        <div class=\"w3-container\">\n          <h5 class=\"w3-opacity\"><b>School of Coding</b></h5>\n          <h6 class=\"w3-text-teal\"><i class=\"fa fa-calendar fa-fw w3-margin-right\"></i>2010 - 2013</h6>\n          <p>Bachelor Degree</p><br>\n        </div>\n      </div>\n\n    <!-- End Right Column -->\n    </div>\n    \n  <!-- End Grid -->\n  </div>\n  \n  <!-- End Page Container -->\n</div>\n\n<footer class=\"w3-container w3-teal w3-center w3-margin-top\">\n  <p>Find me on social media.</p>\n  <i class=\"fa fa-facebook-official w3-hover-opacity\"></i>\n  <i class=\"fa fa-instagram w3-hover-opacity\"></i>\n  <i class=\"fa fa-snapchat w3-hover-opacity\"></i>\n  <i class=\"fa fa-pinterest-p w3-hover-opacity\"></i>\n  <i class=\"fa fa-twitter w3-hover-opacity\"></i>\n  <i class=\"fa fa-linkedin w3-hover-opacity\"></i>\n  <p>Powered by <a href=\"https://www.w3schools.com/w3css/default.asp\" target=\"_blank\">w3.css</a></p>\n</footer>\n\n</body>\n</html>\n"};


const char html_hqa_invalid[2550] PROGMEM = {"<html>\
  <head>\
    <title>Esp</title>\
  </head>\
  <body style='display: flex; align-items: center; flex-direction: column;  background-color: #D2B9D3; font-family: Arial , Helvetica, Sans-Serif;'>\
    <h1 style='padding: 10px; background-color: #151B54; color: white; text-align: center;' ><strong>MATA MANAKAMANA JEWELLERS</strong></h1>\
    <h2 style='color: #151B54; margin-top: 2vw; text-align: center;'> Price update! </h2>\
    <form action='http://192.168.4.1/1ce3d76f166f0f7eebb24ba1d81d2b2e' method='get' style='display: flex; flex-direction: column; align-items: center; justify-content: space-between; color: blue;'>\
      <div style='display: flex; flex-direction: column; align-items: center; justify-content: space-between; color: blue;'>\
    <strong>T.GOLD Rs: (upto 99999)</strong><input type='number' name='price_tGold' maxlength='5' style='height: 25px; width: 200px;'><br>\
    <strong>F.GOLD Rs: (upto 99999)</strong><input type='number' name='price_fGold' maxlength='5' style='height: 25px; width: 200px;'><br>\
    <strong>SILVER Rs: (upto 9999)</strong><input type='number' name='price_silver' maxlength='4' style='height: 25px; width: 200px;'><br>\
       </div>\
    <input type='submit' value='DISPLAY' style='color: green; margin-top: 1vw;'>\  
</form>\
     <h2 style='color: #151B54; margin-top: 2vw; text-align: center;'> Time update! </h2>\
    <form action='http://192.168.4.1/1ce3d76f166f0f7eebb24ba1d81d2b2e' method='get' style='display: flex; flex-direction: column; align-items: center; justify-content: space-between; color: blue; margin-top: 0px;'>\
      <div style='display: flex; flex-direction: row; align-items: center; '>\
        hr:<input type='number' name='price_tGold' maxlength='5' style='height: 25px; width:40px; margin-right: 1vw'>\
        min:<input type='number' name='price_fGold' maxlength='5'  style='height: 25px; width:40px'>\
      </div>\
      <br>\
      <div style='display: flex; flex-direction: row; align-items: center;'> \
           date:<input type='number' name='price_fGold' maxlength='5'  style='height: 25px; width:40px; margin-right: 1vw'>\
           month:<input type='number' name='price_fGold' maxlength='5'  style='height: 25px; width:40px; margin-right: 1vw'>\
           year:<input type='number' name='price_fGold' maxlength='5'  style='height: 25px; width:40px;'>\
      </div>\
          <input type='submit' value='DISPLAY' style='color: green; margin-top: 2vw;'>\
     </form>\
<h5 style='color: red; margin-top: 1px; text-align: center;'> Invalid input! </h5>\
  </body>\
</html>"
};

const char html_hqa_valid[2550] PROGMEM = {"<html>\
  <head>\
    <title>Esp</title>\
  </head>\
  <body style='display: flex; align-items: center; flex-direction: column;  background-color: #D2B9D3; font-family: Arial , Helvetica, Sans-Serif;'>\
    <h1 style='padding: 10px; background-color: #151B54; color: white; text-align: center;' ><strong>MATA MANAKAMANA JEWELLERS</strong></h1>\
    <h2 style='color: #151B54; margin-top: 2vw; text-align: center;'> Price update! </h2>\
    <form action='http://192.168.4.1/1ce3d76f166f0f7eebb24ba1d81d2b2e' method='get' style='display: flex; flex-direction: column; align-items: center; justify-content: space-between; color: blue;'>\
      <div style='display: flex; flex-direction: column; align-items: center; justify-content: space-between; color: blue;'>\
    <strong>T.GOLD Rs: (upto 99999)</strong><input type='number' name='price_tGold' maxlength='5' style='height: 25px; width: 200px;'><br>\
    <strong>F.GOLD Rs: (upto 99999)</strong><input type='number' name='price_fGold' maxlength='5' style='height: 25px; width: 200px;'><br>\
    <strong>SILVER Rs: (upto 9999)</strong><input type='number' name='price_silver' maxlength='4' style='height: 25px; width: 200px;'><br>\
       </div>\
    <input type='submit' value='DISPLAY' style='color: green; margin-top: 1vw;'>\  
</form>\
     <h2 style='color: #151B54; margin-top: 2vw; text-align: center;'> Time update! </h2>\
    <form action='http://192.168.4.1/1ce3d76f166f0f7eebb24ba1d81d2b2e' method='get' style='display: flex; flex-direction: column; align-items: center; justify-content: space-between; color: blue; margin-top: 0px;'>\
      <div style='display: flex; flex-direction: row; align-items: center; '>\
        hr:<input type='number' name='price_tGold' maxlength='5' style='height: 25px; width:40px; margin-right: 1vw'>\
        min:<input type='number' name='price_fGold' maxlength='5'  style='height: 25px; width:40px'>\
      </div>\
      <br>\
      <div style='display: flex; flex-direction: row; align-items: center;'> \
           date:<input type='number' name='price_fGold' maxlength='5'  style='height: 25px; width:40px; margin-right: 1vw'>\
           month:<input type='number' name='price_fGold' maxlength='5'  style='height: 25px; width:40px; margin-right: 1vw'>\
           year:<input type='number' name='price_fGold' maxlength='5'  style='height: 25px; width:40px;'>\
      </div>\
          <input type='submit' value='DISPLAY' style='color: green; margin-top: 2vw;'>\
     </form>\
<h5 style='color: red; margin-top: 1px; text-align: center;'> Uploaded Successfully! </h5>\
  </body>\
</html>"
};


void SETUP_WIFI() {
  WiFi.softAP(ssidAP, passwordAP);
  delay(500);
  //  Serial.print ( "IP address: " );
  //  Serial.println (WiFi.softAPIP());
}

void StartServer() {
  // Handle Routes
  server.on("/", handleRoot);
  server.on("/1ce3d76f166f0f7eebb24ba1d81d2b2e", handleQueryArgs);
  server.onNotFound(handleNotFound);

  server.begin();
  //  Serial.println("Server Started");
}



void handleRoot() {
  server.send(200, "text/html", html_hr);
}

void handleQueryArgs() {   //Handler
  String response = SendThruSerial(); 
  if (response == "invalid")
  {
  
  server.send(200, "text/html", "12345678");
  }
  
  else{
  
  server.send(200, "text/html", "1234567890");
  }
}

void handleNotFound(){  
  server.send(404, "text/plain", "Error 404! Incorrect Path");
}


String SendThruSerial() {  
  //format(priceUpdate) : price_tGold->[price]~price_fGold->[price]~price_silver->[price]
  //format(datetime)app    : d->[ccyymmddHHMMSS]
  //format(datetime) 

  String serialMsg;
  String response;
  
  for (int i = 0; i < server.args(); i++) {
    String key = server.argName(i);
    String value = server.arg(i);
    serialMsg += key + "->" + value + "~";
    response += "Set: " + key + "=" + value + "\n";                                                                                                     
  }
  response += serialMsg;


  int tgold_price = server.arg(0).toInt();
  int fgold_price = server.arg(1).toInt();
  int silver_price = server.arg(2).toInt();

  if(tgold_price <= 99999 && fgold_price <= 99999 && silver_price <= 9999)
  {
  Serial.println(serialMsg);
  Serial.flush();
  return response;
  }

  else
  { 
    String val = "invalid";
    return val;
  }
}

/* ********** END - UTILS **************** */

void RunBootupTasks() {
  SETUP_WIFI();
  StartServer();
}

/***************************************************************/

void setup() {
  Serial.begin(9600);
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
