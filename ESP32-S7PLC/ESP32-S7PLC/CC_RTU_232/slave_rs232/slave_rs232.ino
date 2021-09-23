/*
  ModbusRTU ESP8266/ESP32
  Simple slave example

  (c)2019 Alexander Emelianov (a.m.emelianov@gmail.com)
  https://github.com/emelianov/modbus-esp8266

  modified 13 May 2020
  by brainelectronics

  This code is licensed under the BSD New License. See LICENSE.txt for more info.
*/

#include <ModbusRTU.h>
#include <SoftwareSerial.h>
SoftwareSerial S;
.
int DE_RE = 11; //D4  For MAX485 chip
#define REGN 10
#define SLAVE_ID 1

ModbusRTU mb;

void setup() {
  Serial.begin(9600);
  S.begin(9600, SWSERIAL_8E1, 16, 17);

#if defined(ESP32) || defined(ESP8266)
  mb.begin(&S); //Assing Software serial port to Modbus instance for MAX485 chip having DI,DE,RE,RO Pin at TTL side
#else
  mb.begin(&S); //Assing Software serial port to Modbus instance for MAX485 chip having DI,DE,RE,RO Pin at TTL side
  //mb.begin(&Serial, 16,17);  //or use RX/TX direction control pin (if required)
  mb.setBaudrate(9600);
#endif
  mb.slave(SLAVE_ID);
  mb.addHreg(0, 99, 10);
  //mb.Hreg(REGN, 1234);
}

void loop() {
  mb.task();
  //delay(20);
  yield();
}
