/*TESTING ALLEN BRADLEY CODE with RS232 communication   ESP32
 * MASTER
 * 
 * 
 * TESTING WITH MODBUS LIBRARY AND NO SERIAL 1 IS USED HERE
 * with emlinov library
 * Pranav RS485 cable tested


*/

#include <ModbusRTU.h>
ModbusRTU mb;

#include <SoftwareSerial.h>
SoftwareSerial S(16,17);

int RTS = 11; //D4  For MAX485 chip

//#include "IntToFloat.h"

//D6/D7  (RX , TX)

uint16_t Mread0[10];

bool cbWrite(Modbus::ResultCode event, uint16_t transactionId, void* data) {
#ifdef ESP8266
  Serial.printf_P("Request result: 0x%02X, Mem: %d\n", event, ESP.getFreeHeap());
#elif ESP32
  Serial.printf_P("Request result: 0x%02X, Mem: %d\n", event, ESP.getFreeHeap());
#else
  Serial.print("Request result: 0x");
  Serial.print(event, HEX);
#endif
  return true;
}

void setup() {
  Serial.begin(19200);
  //S.begin(19200, SWSERIAL_8E1,9,10,false,256);
  S.begin(19200, SWSERIAL_8N1);
  //delay(1000);
  //mb.begin(&S,DE_RE); //Assing Software serial port to Modbus instance for MAX485 chip having DI,DE,RE,RO Pin at TTL side
  
  mb.begin(&S); //Assing Software serial port to Modbus instance for MAX485 chip having DI,DE,RE,RO Pin at TTL side
  
  mb.master(); //Assing Modbus function as master
  //Serial.println(); //Print empty line
  //Serial.println(sizeof(Mread0)); //Reaing size of first array
}


void loop() {
  //delay(500);
  if (!mb.slave()) {

    Serial.println("Insidde the LOOP");
    mb.readHreg(1, 0, Mread0, 10 , cbWrite);  //(SlaevID,Address,Buffer,Range of data,Modus call)
    Serial.println(Mread0[0]);
    Serial.println(Mread0[1]);
  }
  mb.task();
  //delay(500);
  yield();
}
