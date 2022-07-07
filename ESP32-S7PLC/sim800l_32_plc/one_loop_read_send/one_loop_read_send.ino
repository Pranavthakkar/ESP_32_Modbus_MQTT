/* Using as in ONE LOOP 
  
   reading s7 and send over mqtt 


  @Pranav 24 nov 14:30
*/



#include "Settimino.h"
#include <Ethernet.h>
#include <SPI.h>

#define TINY_GSM_MODEM_SIM800  // Modem is SIM800L

//For SIM
#include <TinyGsmClient.h>

// For MQTT
#include <PubSubClient.h>
#include <ArduinoJson.h>

TinyGsm modem(Serial2);
TinyGsmClient gsmClient(modem);


PubSubClient mqtt(gsmClient);

// MQTT details
uint32_t lastReconnectAttempt = 0;
int value = 0;
int Size, Result;
byte MyBuffer[1024];
byte MyWrite[4];
long lastMsg = 0;

//==================== Global variable for localRBE=================
StaticJsonDocument<64> prev_value;

char plc_out[150];
//==================== Global variable for localRBE=================

S7Client Client;  // S7 communication

unsigned long Elapsed;  // To calc the execution time


void MarkTime() {
  Elapsed = millis();
}

//----------------------------------------------------------------------
void ShowTime() {
  // Calcs the time
  Elapsed = millis() - Elapsed;
  Serial.print("Job time (ms) : ");
  Serial.println(Elapsed);
}

void CheckError(int ErrNo) {
  Serial.print("Error No. 0x");
  Serial.println(ErrNo, HEX);

  // Checks if it's a Severe Error => we need to disconnect
  if (ErrNo & 0x00FF) {
    Serial.println("S7 ERROR, connecting.");
    //Client.Disconnect();
    Connect();
  }
}
