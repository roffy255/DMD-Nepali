
void setup() 
{ 
 Serial.begin(9600); 
} 

char olle[500];

void loop() 
{ 
 Serial.println(olle);
 olle[0] = 'a';
 olle[1] = 'b';
 olle[2] = 'c';
 olle[3] = 'd';
 olle[4] = 'e';
 olle[5] = 'f';
 olle[5] = '\0';
 Serial.println(olle);
unsigned long ii = micros();
Serial.println(ii);


delay(2000);
if(ii>10000)
  software_Reset() ;

} 

void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
asm volatile ("  jmp 0");  
}  