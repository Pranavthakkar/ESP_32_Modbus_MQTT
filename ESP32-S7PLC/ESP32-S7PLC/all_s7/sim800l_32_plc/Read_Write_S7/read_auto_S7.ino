
/*
  Reading automatically from S7 communication from all tags and
  send it over to mqtt

  project status: On finishing    ("Starting","Intermidiate","On finishing","Finish")

  it is reading as well as writing from mqtt through GPRS.

  Now working with the real application -- GOOD

  nOT USED ANY TASK HERE. 

  NEED TO CHECK WHAT IF SCENARIO:
  1. WHAT IF SUDDENLY PLC CABLE DISCONNECTED
  2. WHAT IF SUDDENLY GPRS DISCONNETED.
  MANY MORE
  
  @pranav 27-11-2021 21:03

*/

#include <SPI.h>
#include <Ethernet.h>
#include "Settimino.h"
#include <ArduinoJson.h>

//TaskHandle_t Task1;
#define TINY_GSM_MODEM_SIM800  // Modem is SIM800L

//For SIM
#include <TinyGsmClient.h>
// For MQTT
#include <PubSubClient.h>


TinyGsm modem(Serial2);
TinyGsmClient gsmClient(modem);


PubSubClient mqtt(gsmClient);

//==================== Global variable for localRBE=================
//StaticJsonDocument<64> prev_value;
DynamicJsonDocument prev_value(3600);
char plc_out[4096];

long lastMsg = 0;
String messageTemp;

String startingString = "T";
//==================== Global variable for localRBE=================


//==================== PLC  =================
uint8_t dbNumber = 3;   //ALWAYS USE THIS FOR YOUR DB NUMBER
int Result;
S7Client Client;  // S7 communication
unsigned long Elapsed;  // To calc the execution time

byte MyBuffer[512];  // ALWAYS USE THIS FOR YOUR CUSTOM BYTES

int maxTags = 162;    // USE THIS FOR HOW MANY TAGS ARE THERE
int maxSize = 434;     // this is used in client db for getting max size of db
//==================== PLC  =================


typedef struct {
  String key;
  int  value;
} offsetDict;


//Predefined {TAGNAME:OFFSET}   const offsetDict myArr[] {
offsetDict myArr[] {

  /*{"T0", 0}, {"T1", 0}, {"T2", 4}, {"T3", 8}, {"T4", 10}, {"T5", 12}, {"T6", 16}, {"T7", 20}, {"T8", 24},
    {"T9", 28}, {"T10", 30}, {"T11", 32}, {"T12", 36}, {"T13", 38}, {"T14", 40}, {"T15", 42}, {"T16", 46},
    {"T17", 47}, {"T18", 48}, {"T19", 50}, {"T20", 52}*/
  {"T0", 0}, {"T1" , 206.0}, {"T2", 208.0}, {"T3", 210.0}, {"T4", 216.0}, {"T5", 214.0}, {"T6", 212.0}, {"T7", 220.0}, {"T8", 222.0},
  {"T9", 230.0}, {"T10", 232.0}, {"T11", 224.0}, {"T12", 226.0}, {"T13", 234.0}, {"T14", 238.0}, {"T15", 228.0}, {"T16", 244.0},
  {"T17", 256.0}, {"T18", 218.0}, {"T19", 258.0}, {"T20", 260.0}, {"T21" , 236.0}, {"T22", 254.0}, {"T23", 252.0}, {"T24", 262.0},
  {"T25", 264.0}, {"T26", 120.0}, {"T27", 122.0}, {"T28", 280.0}, {"T29", 266.0}, {"T30", 268.0}, {"T31", 246.0},  {"T32", 248.0},
  {"T33", 250.0}, {"T34", 132.0}, {"T35", 270.0}, {"T36", 274.0}, {"T37", 276.0}, {"T38", 278.0}, {"T39", 272.0},  {"T40", 288.0},
  {"T41" , 290.0}, {"T42", 292.0}, {"T43", 294.0}, {"T44", 296.0}, {"T45", 134.0}, {"T46", 136.0}, {"T47", 138.0},
  {"T48", 298.0}, {"T49", 300.0}, {"T50", 302.0}, {"T51", 304.0}, {"T52", 306.0}, {"T53", 140.0}, {"T54", 142.0},
  {"T55", 144.0}, {"T56", 308.0}, {"T57", 310.0}, {"T58", 312.0}, {"T59", 314.0}, {"T60", 316.0}, {"T61" , 146.0}, {"T62", 148.0},
  {"T63", 150.0}, {"T64", 318.0}, {"T65", 320.0}, {"T66", 322.0}, {"T67", 324.0}, {"T68", 326.0}, {"T69", 152.0}, {"T70", 154.0},
  {"T71", 156.0}, {"T72", 328.0}, {"T73", 330.0}, {"T74", 332.0}, {"T75", 334.0}, {"T76", 336.0}, {"T77", 158.0}, {"T78", 160.0},
  {"T79", 162.0}, {"T80", 338.0},  {"T81" , 340.0}, {"T82", 342.0}, {"T83", 344.0}, {"T84", 346.0}, {"T85", 164.0}, {"T86", 166.0},
  {"T87", 168.0}, {"T88", 348.0}, {"T89", 350.0}, {"T90", 352.0}, {"T91", 354.0}, {"T92", 356.0}, {"T93", 170.0}, {"T94", 172.0},
  {"T95", 174.0}, {"T96", 358.0}, {"T97", 360.0}, {"T98", 362.0}, {"T99", 364.0}, {"T100", 366.0},  {"T101" , 176.0}, {"T102", 178.0},
  {"T103", 180.0}, {"T104", 386.0}, {"T105", 388.0}, {"T106", 390.0}, {"T107", 392.0}, {"T108", 394.0}, {"T109", 182.0}, {"T110", 184.0},
  {"T111", 186.0}, {"T112", 396.0}, {"T113", 398.0}, {"T114", 400.0}, {"T115", 402.0}, {"T116", 404.0}, {"T117", 188.0}, {"T118", 190.0},
  {"T119", 192.0}, {"T120", 406.0},  {"T121" , 408.0}, {"T122", 410.0}, {"T123", 412.0}, {"T124", 414.0}, {"T125", 194.0},
  {"T126", 196.0}, {"T127", 198.0}, {"T128", 416.0}, {"T129", 418.0}, {"T130", 420.0}, {"T131", 422.0}, {"T132", 424.0}, {"T133", 200.0},
  {"T134", 202.0}, {"T135", 204.0}, {"T136", 368.0}, {"T137", 370.0}, {"T138", 372.0}, {"T139", 374.0}, {"T140", 376.0}, {"T141" , 382.0},
  {"T142", 384.0}, {"T143", 378.0}, {"T144", 380.0}, {"T145", 426.0}, {"T146", 428.0}, {"T147", 430.0}, {"T148", 432.0}, {"T149", 212.0},
  {"T150", 286.0}, {"T151", 282.0}, {"T152", 80.0}, {"T153", 284.0}, {"T154", 114.0}, {"T155", 36.0}, {"T156", 116.0}, {"T157", 38.0},
  {"T158", 118.0}, {"T159", 124.0}, {"T160", 32.0}, {"T161", 34.0}
};




//=======================PUT HERE THE REAL DWORD AND ALL THAT TAGS ===========

String real[] = {};
int maxReal = 0;

String doubleInt[] = {};
int maxDInt = 0;

String number[] = {"T1", "T2", "T3", "T4", "T5", "T6", "T7", "T8", "T9", "T10", "T11", "T12", "T13", "T14", "T15", "T16", "T17", "T18", "T19", "T20",
                   "T21", "T22", "T23", "T24", "T25", "T26", "T27", "T28", "T29", "T30", "T31", "T32", "T33", "T34", "T35", "T36", "T37", "T38", "T39", "T40",
                   "T41", "T42", "T43", "T44", "T45", "T46", "T47", "T48", "T49", "T50", "T51", "T52", "T53", "T54", "T55", "T56", "T57", "T58", "T59", "T60",
                   "T61", "T62", "T63", "T64", "T65", "T66", "T67", "T68", "T69", "T70", "T71", "T72", "T73", "T74", "T75", "T76", "T77", "T78", "T79", "T80",
                   "T81", "T82", "T83", "T84", "T85", "T86", "T87", "T88", "T89", "T90", "T91", "T92", "T93", "T94", "T95", "T96", "T97", "T98", "T99", "T100",
                   "T101", "T102", "T103", "T104", "T105", "T106", "T107", "T108", "T109", "T110", "T111", "T112", "T113", "T114", "T115", "T116", "T117", "T118", "T119", "T120",
                   "T121", "T122", "T123", "T124", "T125", "T126", "T127", "T128", "T129", "T130", "T131", "T132", "T133", "T134", "T135", "T136", "T137", "T138", "T139", "T140",
                   "T141", "T142", "T143", "T144", "T145", "T146", "T147", "T148", "T149", "T150", "T151", "T152", "T153", "T154", "T155", "T156", "T157", "T158", "T159", "T160",
                   "T161"
                  };
int maxNumber = 161;
//String byyte[] = {"T4", "T12", "T14", "T16", "T17"};
// =================== S7 COMMUNICATION ===============



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


bool checkReal(String rr) {

  for (int r = 0; r < maxReal; r++) {
    if (rr == String(real[r]) ) {
      return true;
      break;
    }
  }
  return false;
}

bool checkInt(String ss) {

  for (int n = 0; n < maxNumber; n++) {

    if (ss == String(number[n])) {
      return true;
      break;
    }
  }
  return false;
}


bool checkDInt(String dd) {

  for (int d = 0; d < maxDInt; d++) {
    if ( dd == String(doubleInt[d])) {
      return true;
      break;
    }
  }
  return false;
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
