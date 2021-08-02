//this is working code for modbus master 31 july 7:3 PM

//==============Panav RS485 cable tested

#include <ModbusRTU.h>
ModbusRTU mb;

#include <SoftwareSerial.h>
SoftwareSerial S;

int DE_RE = 4; //D4  For MAX485 chip
//int RX = 16;
//int TX = 17;
#include "IntToFloat.h"

//D6/D7  (RX , TX)

uint16_t Mread0[2];

bool cbWrite(Modbus::ResultCode event, uint16_t transactionId, void* data) {
  Serial.printf_P("Request result: 0x%02X, Mem: %d\n", event, ESP.getFreeHeap());
  return true;
}

void setup() {
  Serial.begin(9600);
  S.begin(9600, SWSERIAL_8N1,16,17,false,256);
  mb.begin(&S, DE_RE); //Assing Software serial port to Modbus instance for MAX485 chip having DI,DE,RE,RO Pin at TTL side
  mb.master(); //Assing Modbus function as master
  Serial.println(); //Print empty line
  Serial.println(sizeof(Mread0)); //Reaing size of first array
}
void loop() {
  if (!mb.slave()) {
    mb.readHreg(1, 0, Mread0, 2 , cbWrite);  //(SlaevID,Address,Buffer,Range of data,Modus call)
    Serial.println(Mread0[0]);
    Serial.println(Mread0[1]);
  }
  mb.task();
  delay(1000);
  yield();
}
