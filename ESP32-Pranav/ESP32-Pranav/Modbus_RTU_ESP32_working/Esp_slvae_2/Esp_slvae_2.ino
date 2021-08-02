#include <ModbusRTU.h>

#define RXD2 16
#define TXD2 17
#define DIR 4
#define REGN 10
#define SLAVE_ID 1

ModbusRTU mb;

void setup() {
  Serial.begin(9600, SERIAL_8N1);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  //mb.begin(&Serial2);
  mb.begin(&Serial2, DIR);
  // Note that second parameter in .begin() is stands for RE_DE pin
  //mb.setBaudrate(9600);
  // No need to .setBaudrate() for ESP devices

  mb.slave(SLAVE_ID);//Initializng modbus slave device with ID 1

  mb.addHreg(REGN); // add the register with address 1
  mb.Hreg(REGN, 12345); // DONT UNDERSTAND WHAT THIS FUNCTION FOR?????
  // Exactly as was wtitten line above assigns value 100 to local HoldingReg #REGN.

  //pinMode(DIR, OUTPUT);    // sets the digital pin 13 as output
  //digitalWrite(13, LOW); // sets the digital pin 13 on
  // No need for lines above the library can control RX/TX by itself
}

//uint16_t storing_data = 0; // place holder to store data

void loop() {
  //mb.readHreg(1, 1, &storing_data, 1, cbWrite);
  // line above make sence only for master.
  mb.task();
  yield();
}
