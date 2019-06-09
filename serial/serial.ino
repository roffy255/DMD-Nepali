void setup() {
 Serial.begin(9600); // put your setup code here, to run once:

}

void loop() {
  while(Serial.available())
  {
    String serial =   Serial.readString();
    Serial.println(serial);
  }
 

}
