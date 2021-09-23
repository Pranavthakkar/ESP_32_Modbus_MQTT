/*
  ModbusRTU ESP8266/ESP32

  working good 22-9 10:30

  using it as master like hardware serial

  working use tx -- tx and rx -- rx
  23-9 13:09


*/

#include <ModbusRTU.h>

#include <SoftwareSerial.h>


int RTS = 7; //D4  For MAX485 chip
// SoftwareSerial S(D1, D2, false, 256);

// receivePin, transmitPin, inverse_logic, bufSize, isrBufSize
// connect RX to D2 (GPIO4, Arduino pin 4), TX to D1 (GPIO5, Arduino pin 4)
SoftwareSerial S(16, 17);


ModbusRTU mb;

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
  Serial.begin(9600);
#if defined(ESP8266)
  S.begin(19200, SWSERIAL_8N1);
  mb.begin(&S);
#elif defined(ESP32)
  Serial2.begin(19200, SERIAL_8N1);       // No Parity
  mb.begin(&Serial2);
#else
  Serial2.begin(19200, SERIAL_8N1);   // No Parity
  mb.begin(&Serial2);
  //mb.setBaudrate(9600);
#endif
  mb.master();
}

//bool coils[20];
uint16_t Mread0[10];  // if you put it global it will gonna save value of it in case of cable failure


void loop() {
  if (!mb.slave()) {
    //mb.readCoil(1, 1, coils, 20, cbWrite);
    mb.readHreg(1, 0, Mread0, 10 , cbWrite);  //(SlaevID,Address,Buffer,Range of data,Modus call)

    Serial.println(Mread0[0]);
    Serial.println(Mread0[1]);
    
    Serial.println(Mread0[2]);
    Serial.println(Mread0[3]);
  }
  mb.task();
  yield();
  delay(50);
}
