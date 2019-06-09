void setup() {
  Serial.begin(9600);

}

String cmd_val = "";

void loop() {
  String key = "M";
  String value = "kldjskafjlksdjkfjhkhgjgjkhhkjhfjkldshfjkladjkfhajdklhfjkldshljfkhldajskhfkjhadjkhlfjkhsdjkfhljkadshjkfhlkjdshkfjhadsjhflkjdshkjflhadshfkjladshlfjksadhjklfadsjkl fhkadjhjadsklhnadshfjkhfdgjkhafkdjlgkj gjkhdfjkdhaskjfhkjsadhfjklhadsjklhfkjlhajskldhjklfhkasdjlhfkjhasdkjlfhkjalsdhfkjlhdsajklfhkjladskjlfhdasjkfh kadsjfhkadsjhfkjladshfkjlhdasjklfhkjsdlahkfdsjklfhasdkjlfhjkdashkfjhadsjklfhkjsdhgkhdaskjlhhriueahjkfbadsjkghljadhfkhdsajklghkjladhgjkldkljafhkjdaskljghkdasjlgkljdashkjlhdasjkfhkjsdahfjkladshfjkadskjlfhadsjklhiuawehlkjfkjdsbflkdasj fjadskfhkjadsfhkjladshgjkadshkjlghdskjlfhakdlhfljkadhgkadshkjghadksjlhkadsjlhlkfjadshkjlfhjadskhkjladshfjkadshlkjhadsjklghjkadhkhakdjlfakadshnfljadshfjkladhnfhndaskjadshjkfhkjadskjfhjkadshfkjhasdkjfhkjadshkljfhkljdshfjklhdskjfhlkjdsahfjkdagjahdskljghaljkdhfjklhadskljghajkldshfjkhsdakjlhfjkahsdkjfhjksdahfkjadsjkhfkjdafhjhdaksjlfadshljkfhkjladshkljfhkadsjlhkljads";
    if(key == "M" & value.length() > 100){
      int val_len = value.length();
      int div;
      if(val_len % 100 == 0){
        div = (val_len / 100);
      }else{
        div = (val_len / 100) + 1;
      }
      if(val_len > 100){
         Serial.println("M->" + value.substring(0,100) + "~");
         Serial.flush();
         value = value.substring(100,value.length());
//         delay(1000);
      while(!(cmd_val == "a"))
      {
        cmd_val == Serial.readString();
      }
      cmd_val == "b";
      Serial.flush();
         for(int i = 1; i < div; i++){
         Serial.println("->" + value.substring(0,100) + "~");
         Serial.flush();
         value = value.substring(100,value.length());
//         delay(1000);
           while(!(cmd_val == "a"))
      {
        cmd_val == Serial.readString();
      }
      cmd_val == "b";
          }
      }   
      }
      else{
      String  serialMsg = key + "->" + value + "~";
        Serial.println(serialMsg);
        Serial.flush();
      }
  Serial.flush();
  }
