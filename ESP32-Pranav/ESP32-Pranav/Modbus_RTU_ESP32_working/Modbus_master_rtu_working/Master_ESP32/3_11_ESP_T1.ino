// MOdbus RTU Testing wih MAX 485 CHip
// COnnection>. ESP 32 (15) ----> DE+RE
//              ESP 32 (16) -----> RX
//              ESP 32 (17) -----> TX
//              ESP 32 (VIN) -----> 5 V
//              ESP 32 (GND) -----> GND

// I've used Serial 2 port where it is by default in ESP 32 as 16 RX and 17 TX

// Working with modbus write on register 2 with value 513, & 512 on 25 nov 2022

// Now working with analog input connected to 3.3 v to gnd and input to gpio 34
// @Pranav and @Rutvik
// Working good 29_nov _2022


#include <ModbusRTU.h>
#include <SoftwareSerial.h>
SoftwareSerial S;

#define RXD2 16 //RO
#define TXD2 17 //DI

#define Switch 26 //Digital Input Forward
#define Switch_rev 27  // Digital Input Rev

bool stat;
bool status_rev;
int DE_RE = 15; //D4  For MAX485 chip

ModbusRTU mb;

uint16_t Mread0[1];

int sensorPin = 34;   // Analog Input
int sensorValue;
int motorValue;

//
bool cbWrite(Modbus::ResultCode event, uint16_t transactionId, void* data) {

  if (event != Modbus::EX_SUCCESS)
  {
    Serial.printf_P("Request result: 0x%02X, Mem: %d\n", event, ESP.getFreeHeap());
  }
  //Serial.printf_P("Request result: 0x%02X, Mem: %d\n", event, ESP.getFreeHeap());
  return true;
}

void setup() {

  Serial.begin(9600);
  //S.begin(9600, SWSERIAL_8N1,16,17);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);    // Working
  mb.begin(&Serial2, 15); //Assing Software serial port to Modbus instance for MAX485 chip having DI,DE,RE,RO Pin at TTL side
  mb.master(); //Assing Modbus function as master
  Serial.println(); //Print empty line
  pinMode(Switch, INPUT);
  //Serial.println(sizeof(Mread0)); //Reaing size of first array
  mb.addReg(HREG(10));     // Add Holding register from #100 with out length
}

void loop() {

  if (!mb.slave()) {

    sensorValue = analogRead(sensorPin);
    //Serial.println(sensorValue);
    motorValue = map(sensorValue, 0, 4095, 0, 4800);

    //Serial.println("Motor Valueeeeeeeeeeeeeeee");
    //Serial.println(motorValue);
    //    mb.readHreg(1, 4, Mread0, 1 , cbWrite);  //(SlaevID,Address,Buffer,Range of data,Modus call)
    stat = digitalRead(Switch);
    status_rev = digitalRead(Switch_rev);

    if (stat == true) {
      //Serial.println("Motor Starting");
      mb.writeHreg(1, 2, 513, cbWrite);
      while (mb.slave()) {
        mb.task();
        yield();
      }
      mb.writeHreg(1, 4, motorValue, cbWrite);   //uint16_t writeHreg(uint8_t slave_id, uint16_t offset, uint16_t value, cbTransaction cb = nullptr);
      while (mb.slave()) {
        mb.task();
        yield();
      }
      //mb.readHreg(1, 4, Mread0, 1 , cbWrite);  //(SlaevID,Address,Buffer,Range of data,Modus call)
      //Serial.println("Actual Speed of the Motor ");
      //Serial.println(Mread0[0]);
      //      while (mb.slave()) {
      //        mb.task();
      //        yield();
      //      }
    }
    // Reverse Procedure
    else if (status_rev == true) {
      mb.writeHreg(1, 2, 521, cbWrite);
      while (mb.slave()) {
        mb.task();
        yield();
      }
      mb.writeHreg(1, 4, motorValue, cbWrite);   //uint16_t writeHreg(uint8_t slave_id, uint16_t offset, uint16_t value, cbTransaction cb = nullptr);
      while (mb.slave()) {
        mb.task();
        yield();
      }
    }
    else {
      mb.writeHreg(1, 2, 512, cbWrite);   //uint16_t writeHreg(uint8_t slave_id, uint16_t offset, uint16_t value, cbTransaction cb = nullptr);
    }


    while (mb.slave()) {
      mb.task();
      yield();
    }
  }
  mb.task();
  yield();
}
