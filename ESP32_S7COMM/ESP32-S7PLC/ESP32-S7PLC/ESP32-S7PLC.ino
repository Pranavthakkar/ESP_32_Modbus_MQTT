/*----------------------------------------------------------------------
Data Read Demo

 Created 19 Nov 2013
 by Davide Nardella
 
------------------------------------------------------------------------

----------------------------------------------------------------------*/
#include <SPI.h>
#include <Ethernet.h>
#include "Settimino.h"

// Uncomment next line to perform small and fast data access
#define DO_IT_SMALL
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0F, 0x08, 0xE11 };
  
IPAddress Local(192,168,178,55); // Local Address
IPAddress PLC(192,168,178,30);   // PLC Address

byte MyBuffer[1024];
byte MyWrite[4];

S7Client Client;

//----------------------------------------------------------------------
// Setup : Init Ethernet and Serial port
//----------------------------------------------------------------------
void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
     while (!Serial) {
      ; // wait for serial port to connect. Needed for Leonardo only
    }
    // Start the Ethernet Library
    Ethernet.begin(mac, Local);
    // Setup Time, someone said me to leave 2000 because some 
    // rubbish compatible boards are a bit deaf.
    delay(2000); 
    Serial.println(Ethernet.localIP());
}
//----------------------------------------------------------------------
// Connects to the PLC
//----------------------------------------------------------------------
bool Connect()
{
    int Result=Client.ConnectTo(PLC, 
                                  0,  // Rack (see the doc.)
                                  2); // Slot (see the doc.)
    Serial.print("Connecting to ");Serial.println(PLC);  
    if (Result==0) 
    {
      Serial.print("Connected ! PDU Length = ");Serial.println(Client.GetPDULength());
    }
    else
      Serial.println("Connection error");
    return Result==0;
}



 //----------------------------------------------------------------------
// Main Loop
//----------------------------------------------------------------------
void loop() 
{
  
  int Size, Result;
  float Pressure;
  unsigned long Encoder;
  int Component;
  byte Bytes;
 
   

  
  // Connection
  while (!Client.Connected)
  {
    if (!Connect())
      delay(500);
  }
  
  Serial.print("Reading "); 

  Result=Client.ReadArea(S7AreaDB, // We are requesting DB access
                         307,        // DB Number = 1
                         0,        // Start from byte N.0
                         11,     // We need "Size" bytes
                         &MyBuffer);  // Put them into our target (Buffer or PDU)
  if (Result==0)
  {
    Pressure = S7.FloatAt(&MyBuffer, 0);
    Serial.print("Pressure=");
    Serial.println(Pressure); 
    Encoder = S7.DWordAt(&MyBuffer, 4);
    Serial.print("Encoder=");
    Serial.println(Encoder);
    Component = S7.IntegerAt(&MyBuffer, 8);
    Serial.print("Component=");
    Serial.println(Component); 
    Bytes = S7.ByteAt(&MyBuffer, 10);
    Serial.print("Bytes=");
    Serial.println(Bytes);  
  }
  else
    CheckError(Result);
    
    float f = 123.45;
    byte* bytes = (byte*)&f;
     
  
    
    
    Result=Client.WriteArea(S7AreaDB, // We are requesting DB access
                       307,        // DB Number = 1
                         0,        // Start from byte N.0
                         4,     // We need "Size" bytes
                         bytes);  // Put them into our target (Buffer or PDU)
    
    
    
  delay(500);  
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
