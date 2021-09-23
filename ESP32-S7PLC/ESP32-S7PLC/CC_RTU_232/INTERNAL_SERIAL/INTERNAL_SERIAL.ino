 #define RXD2 16
 #define TXD2 17
 
 void setup() {
   Serial.begin(9600);
   Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
   delay(1000);
   Serial.println("Loopback program started");
   pinMode(7,OUTPUT);
 }
 void loop() {
   if(Serial.available()){
     Serial.write("-");
     Serial2.write(Serial.read());  
   }
   if(Serial2.available()){
     Serial.write(".");
     
     Serial.write(Serial2.read());  
   }
   Serial2.write("sd");
   digitalWrite(7,HIGH);
   
   delay(10000);
   digitalWrite(7,LOW);
 }
