/*
  Modbus-Arduino Example - Publish multiple DI as coils (Modbus IP ESP8266/ESP32)

  Original library
  Copyright by André Sarmento Barbosa
  http://github.com/andresarmento/modbus-arduino

  Current version
  (c)2018 Alexander Emelianov (a.m.emelianov@gmail.com)
  https://github.com/emelianov/modbus-esp8266
*/

#include <SPI.h>
#include <Ethernet.h>       // Ethernet library v2 is required
#include <ModbusEthernet.h>

//Used Pins
#ifdef ESP8266
uint8_t pinList[] = {D0, D1, D2, D3, D4, D5, D6, D7, D8};
#else //ESP32
uint8_t pinList[] = {12, 13, 14, 14, 16, 17, 18, 21, 22, 23};
#endif

#define LEN sizeof(pinList)/sizeof(uint8_t)

#define COIL_BASE 0
//ModbusIP object

#define RESET_P 26        // Tie the Wiz820io/W5500 reset pin to ESP32 GPIO26 pin.
// Enter a MAC address and IP address for your controller below.
uint8_t eth_MAC[] = { 0x02, 0xF0, 0x0D, 0xBE, 0xEF, 0x01 };

IPAddress remote(10, 0, 0, 178); // Address of Modbus Slave device

IPAddress eth_IP(10, 0, 0, 178);    // *** CHANGE THIS to something relevant for YOUR LAN. ***
IPAddress eth_MASK(255, 255, 255, 0);   // Subnet mask.
IPAddress eth_DNS(8, 8, 8, 8);    // *** CHANGE THIS to match YOUR DNS server.           ***
IPAddress eth_GW(10, 0, 0, 1);   // *** CHANGE THIS to match YOUR Gateway (router).     ***

ModbusEthernet mb;

// Callback function to read corresponding DI
uint16_t cbRead(TRegister* reg, uint16_t val) {
  // Checking value of register address which callback is called on.
  // See Modbus.h for TRegister and TAddress definition
  if (reg->address.address < COIL_BASE)
    return 0;
  uint8_t offset = reg->address.address - COIL_BASE;
  if (offset >= LEN)
    return 0;
  return COIL_VAL(digitalRead(pinList[offset]));
}
// Callback function to write-protect DI
uint16_t cbWrite(TRegister* reg, uint16_t val) {
  return reg->value;
}

// Callback function for client connect. Returns true to allow connection.
bool cbConn(IPAddress ip) {
  Serial.println(ip);
  return true;
}

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
  Serial.begin(115200);
  Ethernet.init(5);        // SS pin
  WizReset();

  Serial.println("Starting ETHERNET connection...");
  //Ethernet.begin(mac, ip);  // start the Ethernet connection
  Ethernet.begin(eth_MAC, eth_IP, eth_DNS, eth_GW, eth_MASK);

  delay(2000);              // give the Ethernet shield a second to initialize

  Serial.print("Ethernet IP is: ");
  Serial.println(Ethernet.localIP());

  /*
     Sanity checks for W5500 and cable connection.
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

  for (uint8_t i = 0; i < LEN; i++)
    pinMode(pinList[i], INPUT);
  mb.onConnect(cbConn);   // Add callback on connection event
  mb.server();

  mb.addHreg(COIL_BASE, COIL_VAL(false), LEN); // Add Coils.
  mb.onGetCoil(COIL_BASE, cbRead, LEN);  // Add single callback for multiple Coils. It will be called for each of these coils value get
  mb.onSetCoil(COIL_BASE, cbWrite, LEN); // The same as above just for set value
}

void loop() {
  //Call once inside loop() - all magic here
  mb.task();
  delay(1000);
}
