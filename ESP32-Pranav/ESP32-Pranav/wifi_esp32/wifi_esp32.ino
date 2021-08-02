#include "ModbusMaster.h" //https://github.com/4-20ma/ModbusMaster

/*!
  We're using a MAX485-compatible RS485 Transceiver.
  Rx/Tx is hooked up to the hardware serial port at 'Serial'.
  The Data Enable (DE) and Receiver Enable (RE) pins are hooked up as follows:
*/

#define MAX485_RE_NEG  4 //D4 RS485 has a enable/disable pin to transmit or receive data. Arduino Digital Pin 2 = Rx/Tx 'Enable'; High to Transmit, Low to Receive
#define Slave_ID       1
#define RX_PIN      16 //RX2 
#define TX_PIN      17  //TX2 

// instantiate ModbusMaster object
ModbusMaster modbus;

void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, HIGH); //Switch to transmit data
}

void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, LOW); //Switch to receive data
}

void setup()
{
  pinMode(MAX485_RE_NEG, OUTPUT);

  // Init in receive mode
  digitalWrite(MAX485_RE_NEG, LOW);

  // Modbus communication runs at 9600 baud
  Serial.begin(9600, SERIAL_8N1);

  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  modbus.begin(Slave_ID, Serial2);

  // Callbacks allow us to configure the RS485 transceiver correctly
  modbus.preTransmission(preTransmission);
  modbus.postTransmission(postTransmission);
}


  
 
bool state = true;

void loop()
{
  uint8_t result;
  uint16_t data[6];
  
  // Toggle the coil at address 0x0002 (Manual Load Control)
 // result = node.writeSingleCoil(0x0002, state);
  //state = !state;

  // Read 16 registers starting at 0x3100)
  result = modbus.readInputRegisters(0x3100, 16);
  if (result == modbus.ku8MBSuccess)
  {
    
    Serial.println(modbus.getResponseBuffer(0));
    Serial.println(modbus.getResponseBuffer(1));
    Serial.println(modbus.getResponseBuffer(2));
    Serial.println(modbus.getResponseBuffer(3));
    Serial.println(modbus.getResponseBuffer(4));
    Serial.println(modbus.getResponseBuffer(5));
    Serial.println(modbus.getResponseBuffer(6));
    Serial.println(modbus.getResponseBuffer(7));
  }

  delay(1000);
}
