/*READING WITH LAN CABLE TO THE PLC SUCCESSFULLY
   ---  @PRANAV  31/8/2021  21:43
   USE THIS CODE ALWAYS Sending RTC on DB4 and in int of year month format
   Also Using RTC Here
  ----------------------------------------------------------------------*/

#include <SPI.h>
#include <Ethernet.h>
#include <DS3231.h>
#include "Settimino.h"


//#define RESET_P 26        // Tie the Wiz820io/W5500 reset pin to ESP32 GPIO26 pin.
// Uncomment next line to perform small and fast data access
#define DO_IT_SMALL

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

byte mac[] = { 0x02, 0xF0, 0x0D, 0xBE, 0xEF, 0x01 };             /// Mac changed


IPAddress PLC(172, 17, 1, 123); // PLC Address

IPAddress ip(172, 17, 1, 180); // Local Address



byte MyBuffer[20];

Time t;
S7Client Client;


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

  rtc.begin();
  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(TUESDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(22, 18, 20);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(31, 8, 2021);   // Set the date to January 1st, 2014

  Ethernet.init(10);         // SS pin
  //WizReset();
  Serial.begin(115200);
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
// Main Loop
//----------------------------------------------------------------------
void loop()
{

  int  Result;
  float Pressure;

  t = rtc.getTime();

  // Send time
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");

  // Send time
  Serial.println(rtc.getTimeStr());
  //Serial.println(t.year, DEC);
  //Serial.println(t.date, DEC);
  //Serial.println(t.mon, DEC);

  //Serial.println(t.hour, DEC);
  //Serial.println(t.min, DEC);

  int yea = t.year;
  uint8_t month = t.mon;
  uint8_t day = t.date;
  uint8_t hou = t.hour;
  uint8_t minute = t.min;

  // Connection
  while (!Client.Connected)
  {
    if (!Connect())
      delay(500);
  }

  Serial.print("Reading ");

  S7.SetIntAt(&MyBuffer, 0, yea);   // Year at first
  S7.SetIntAt(&MyBuffer, 2, month);   // month at 2
  S7.SetIntAt(&MyBuffer, 4, day);   // day at 3
  S7.SetIntAt(&MyBuffer, 6, hou);   // hou at 4
  S7.SetIntAt(&MyBuffer, 8, minute);   // minute at 5
  

  Result = Client.WriteArea(S7AreaDB, // We are requesting DB access
                            4,        // DB Number = 1
                            0,        // Start from byte N.0
                            10,     // We need "Size" bytes
                            &MyBuffer);  // Put them into our target (Buffer or PDU)
  if (Result == 0)
  {
    //Serial.println("Send Successfully INT ");
    Serial.println("-----------date send--------");

  }

  else {
    CheckError(Result);
  }
 
  delay(1000);
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
    Connect();
  }
}
