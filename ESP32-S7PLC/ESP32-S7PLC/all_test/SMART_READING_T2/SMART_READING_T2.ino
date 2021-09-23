/*READING WITH LAN CABLE TO THE S7 200 SMART PLC WORKING for writing single BIT 
   ---  @PRANAV  25/8/2021  15:15
   USE THIS CODE ALWAYS

  ----------------------------------------------------------------------
  Data Read Demo

  Created 19 Nov 2013
  by Davide Nardella
  ------------------------------------------------------------------------
  ----------------------------------------------------------------------*/

#include <SPI.h>
#include <Ethernet.h>

#include "Settimino.h"


#define RESET_P 26        // Tie the Wiz820io/W5500 reset pin to ESP32 GPIO26 pin.
// Uncomment next line to perform small and fast data access
#define DO_IT_SMALL

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
// Enter a MAC address and IP address for your controller below.

byte mac[] = { 0x02, 0xF0, 0x0D, 0xBE, 0xEF, 0x01 };             /// Mac changed


IPAddress PLC(10, 0, 0, 172); // PLC Address

IPAddress ip(10, 0, 0, 178); // Local Address



byte MyBuffer[1024];
byte MyWrite[4];

S7Client Client;

unsigned long Elapsed; // To calc the execution time

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



//----------------------------------------------------------------------
// Setup : Init Ethernet and Serial port
//----------------------------------------------------------------------

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);


  Ethernet.init(5);         // SS pin
  WizReset();

  Serial.println("Starting ETHERNET connection...");
  Ethernet.begin(mac, ip);  // start the Ethernet connection

  delay(1000);              // give the Ethernet shield a second to initialize

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
}


//----------------------------------------------------------------------
// Connects to the PLC
//----------------------------------------------------------------------
bool Connect()
{
  int Result = Client.ConnectTo(PLC,
                                0,  // Rack (see the doc.)
                                1); // Slot (see the doc.)
  Serial.print("Connecting to "); Serial.println(PLC);
  if (Result == 0)
  {
    Serial.print("Connected ! PDU Length = "); Serial.println(Client.GetPDULength());
  }
  else
    Serial.println("Connection error");
  return Result == 0;
}


//----------------------------------------------------------------------
// Profiling routines
//----------------------------------------------------------------------
void MarkTime()
{
  Elapsed = millis();
}

//----------------------------------------------------------------------
void ShowTime()
{
  // Calcs the time
  Elapsed = millis() - Elapsed;
  Serial.print("Job time (ms) : ");
  Serial.println(Elapsed);
}






//----------------------------------------------------------------------
// Main Loop
//----------------------------------------------------------------------
void loop()
{

  int Size, Result;
  bool Pressure;
  bool Encoder;
  bool Component;
  byte Bytes;

  // Connection
  while (!Client.Connected)
  {
    if (!Connect())
      delay(500);
  }

  Serial.print("Reading ");

  MarkTime();

  //=============================WRITING ==================


  Result = Client.WriteBit(S7AreaDB, 1, 0, 2, 1); 
  Result = Client.WriteBit(S7AreaDB, 1, 0, 3, 0);
  Result = Client.WriteBit(S7AreaDB, 1, 1, 6, 1);
  
  /*Result = Client.ReadArea(S7AreaDB, // We are requesting DB access
                           1,        // DB Number = 1
                           0,        // Start from byte N.0
                           22,     // We need "Size" bytes
                           &MyBuffer);  // Put them into our target (Buffer or PDU)*/

  Serial.println("Result INTEGER >>>>>>>>");
  Serial.println(Result);

  if (Result == 0)
  {
    
    Serial.println("Send Successfully BIT ");
    Serial.println("-----------INTEGER Success--------");
  }

  /*if (Result == 0)
    {
    ShowTime();
    Pressure = S7.BitAt(&MyBuffer, 0, 2);
    Serial.print("_0_2_bit_=");
    Serial.println(Pressure);
    Serial.println("");
    Serial.println("");
    Encoder = S7.BitAt(&MyBuffer, 0, 6);
    Serial.print("_0_6_bit_=");
    Serial.println(Encoder);
    Serial.println("");
    Serial.println("");
    Component = S7.BitAt(&MyBuffer, 1, 6);
    Serial.print("_1_6_bit_=");
    Serial.println(Component);
    Serial.println("");
    Serial.println("");
    Serial.println("");
    Serial.println("");
    }*/

  else
    CheckError(Result);

  
  delay(1500);
}

//----------------------------------------------------------------------
// Prints the Error number
//----------------------------------------------------------------------
void CheckError(int ErrNo)
{
  Serial.print("Error No. 0x");
  Serial.println(ErrNo, HEX);

  // Checks if it's a Severe Error => we need to disconnect
  if (ErrNo & 0x00FF)
  {
    Serial.println("SEVERE ERROR, disconnecting.");
    Client.Disconnect();
  }
}
