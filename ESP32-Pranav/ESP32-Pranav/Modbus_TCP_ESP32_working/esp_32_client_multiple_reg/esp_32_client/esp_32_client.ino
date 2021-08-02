/* Modbus Master Multiple Resistor done trail.. 
 *  
 *  1 Aug 2021 15:23 PM trail success
 */


/*
  Modbus-Arduino Example - Master Modbus IP Client (ESP8266/ESP32)
  Read Holding Register from Server device

  (c)2018 Alexander Emelianov (a.m.emelianov@gmail.com)
  https://github.com/emelianov/modbus-esp8266
*/

#ifdef ESP8266
 #include <ESP8266WiFi.h>
#else
 #include <WiFi.h>
#endif
#include <ModbusIP_ESP8266.h>

const int REG = 0;               // Modbus Hreg Offset
IPAddress remote(192, 168, 1, 103);  // Address of Modbus Slave device
const int LOOP_COUNT = 10;

ModbusIP mb;  //ModbusIP object

void setup() {
  Serial.begin(115200);
 
  WiFi.begin("DIGIMECK ENGINEERS", "DIGI@2019");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  mb.client();
}

uint16_t res[10];
uint8_t show = LOOP_COUNT;

void loop() {
  if (mb.isConnected(remote)) {   // Check if connection to Modbus Slave is established
    mb.readHreg(remote, REG, res,10);  // Initiate Read Coil from Modbus Slave

    Serial.print("Register 3 :");
    Serial.println(res[3]);

    //Serial.println(" ");
    //Serial.println(" ");
    
    //Serial.print("Register 4 :");
    //Serial.println(res[4]);

    //Serial.println(" ");
    //Serial.println(" ");
    
    //Serial.print("Register 5 :");
    //Serial.println(res[5]);

    //Serial.println(" ");
    //Serial.println(" ");
    //mb.readHreg(remote, REG, &res);  // Initiate Read Coil from Modbus Slave
  } else {
    mb.connect(remote);           // Try to connect if no connection
  }
  mb.task();                      // Common local Modbus task
  delay(1);                     // Pulling interval
  if (!show--) {                   // Display Slave register value one time per second (with default settings)
    //Serial.println(res);
    
    show = LOOP_COUNT;
    
    //Serial.print("Show");
    //Serial.println(show);
    
  }
}
