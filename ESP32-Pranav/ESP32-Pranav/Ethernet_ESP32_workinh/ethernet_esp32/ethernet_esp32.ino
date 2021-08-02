/*
  ModbusTCP for W5x00 Ethernet library
  Basic Client code example

  (c)2020 Alexander Emelianov (a.m.emelianov@gmail.com)
  https://github.com/emelianov/modbus-esp8266

  This code is licensed under the BSD New License. See LICENSE.txt for more info.
*/

#include <SPI.h>
#include <Ethernet.h>       // Ethernet library v2 is required
#include <ModbusEthernet.h>

#define RESET_P 26        // Tie the Wiz820io/W5500 reset pin to ESP32 GPIO26 pin.


const uint16_t REG = 0;               // Modbus Hreg Offset


IPAddress remote(192, 168, 1, 111);  // Address of Modbus Slave device
const int32_t showDelay = 500;   // Show result every n'th mellisecond

// Enter a MAC address and IP address for your controller below.
byte mac[] = { 0x02, 0xF0, 0x0D, 0xBE, 0xEF, 0x01 };             /// Mac changed

IPAddress ip(192, 168, 1, 178); // The IP address will be dependent on your local network:



ModbusEthernet mb;               // Declare ModbusTCP instance

void WizReset() {
    Serial.print("Resetting Wiz W5500 Ethernet Board...  ");
    pinMode(RESET_P, OUTPUT);
    digitalWrite(RESET_P, HIGH);
    delay(250);
    digitalWrite(RESET_P, LOW);
    delay(50);
    digitalWrite(RESET_P, HIGH);
    delay(350);
    Serial.println("Done.");
}


void prt_hwval(uint8_t refval) {
    switch (refval) {
    case 0:
        Serial.println("No hardware detected.");
        break;
    case 1:
        Serial.println("WizNet W5100 detected.");
        break;
    case 2:
        Serial.println("WizNet W5200 detected.");
        break;
    case 3:
        Serial.println("WizNet W5500 detected.");
        break;
    default:
        Serial.println
            ("UNKNOWN - Update espnow_gw.ino to match Ethernet.h");
    }
}


void prt_ethval(uint8_t refval) {
    switch (refval) {
    case 0:
        Serial.println("Uknown status.");
        break;
    case 1:
        Serial.println("Link flagged as UP.");
        break;
    case 2:
        Serial.println("Link flagged as DOWN. Check cable connection.");
        break;
    default:
        Serial.println
            ("UNKNOWN - Update espnow_gw.ino to match Ethernet.h");
    }
}

void setup() {
  Serial.begin(115200);     // Open serial communications and wait for port to open
  #if defined(AVR_LEONARDO)
  while (!Serial) {}        // wait for serial port to connect. Needed for Leonardo only
  #endif
  Ethernet.init(5);         // SS pin
  WizReset();

  Serial.println("Starting ETHERNET connection...");
  Ethernet.begin(mac, ip);  // start the Ethernet connection
  
  delay(1000);              // give the Ethernet shield a second to initialize
  
  Serial.print("Ethernet IP is: ");
  Serial.println(Ethernet.localIP());

  /*
     * Sanity checks for W5500 and cable connection.
     */
    Serial.print("Checking connection.");
    bool rdy_flag = false;
    for (uint8_t i = 0; i <= 20; i++) {
        if ((Ethernet.hardwareStatus() == EthernetNoHardware)
            || (Ethernet.linkStatus() == LinkOFF)) {
            Serial.print(".");
            rdy_flag = false;
            delay(80);
        } else {
            rdy_flag = true;
            break;
        }
    }
    if (rdy_flag == false) {
        Serial.println
            ("\n\r\tHardware fault, or cable problem... cannot continue.");
        Serial.print("Hardware Status: ");
        prt_hwval(Ethernet.hardwareStatus());
        Serial.print("   Cable Status: ");
        prt_ethval(Ethernet.linkStatus());
        while (true) {
            delay(10);          // Halt.
        }
    } else {
        Serial.println(" OK");
    }


  
  mb.client();              // Act as Modbus TCP server
}

//uint16_t res = 0;

uint16_t res[10];         // change res = 0
uint32_t showLast = 0;




void loop() {
if (mb.isConnected(remote)) {   // Check if connection to Modbus Slave is established
  //Serial.printf("Reading");
    mb.readHreg(remote, REG, res,10);  // Initiate Read Hreg from Modbus Slave
    //Serial.println("COnnected");
  } else {
    mb.connect(remote);           // Try to connect if not connected
    //Serial.println("Reconnecting");
  }
  delay(100);                     // Pulling interval
  mb.task();                      // Common local Modbus task
  if (millis() - showLast > showDelay) { // Display register value every 5 seconds (with default settings)
    showLast = millis();
    //Serial.println(res);
  }
}
