/* reading s7200 st30 and s71215 both db from it
 *  
 *  Use Semaphore here it is working good.
 *  for making s7 communicatation better without an error
 *  
 *  Also reading from st 30 and sending over to s71200 vice versa.
 *  

   ---  @PRANAV 25/8/2021  19:20

  ----------------------------------------------------------------------
  Data Read Demo

  Created 19 Nov 2013
  by Davide Nardella
  ------------------------------------------------------------------------
  ----------------------------------------------------------------------*/

TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;

//#include "Platform.h"
#include "Settimino.h"
#include <Ethernet.h>
#include <SPI.h>

SemaphoreHandle_t mate_1200;

SemaphoreHandle_t mate_200;

#define RESET_P 26        // Tie the Wiz820io/W5500 reset pin to ESP32 GPIO26 pin.
// Uncomment next line to perform small and fast data access
#define DO_IT_SMALL

//Mqtt
//#include <PubSubClient.h>
//#include <ArduinoJson.h>


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:

uint8_t eth_MAC[] = { 0x02, 0xF0, 0x0D, 0xBE, 0xEF, 0x01 };

// The IP address will be dependent on your local network:
//byte mac[] = {
//  0x90, 0xA2, 0xDA, 0x0F, 0x08, 0xE1 };

IPAddress S7_1215(10, 0, 0, 174); // PLC Address S7 1215
IPAddress S7_200(10, 0, 0, 172); // PLC Address S7 200

IPAddress eth_IP(10, 0, 0, 178);    // *** CHANGE THIS to something relevant for YOUR LAN. ***
IPAddress eth_MASK(255, 255, 255, 0);   // Subnet mask.
IPAddress eth_DNS(8, 8, 8, 8);    // *** CHANGE THIS to match YOUR DNS server.           ***
IPAddress eth_GW(10, 0, 0, 1);   // *** CHANGE THIS to match YOUR Gateway (router).     ***

S7Client Client_1200;
S7Client Client_200;

int connected_1200;

unsigned long Elapsed; // To calc the execution time



//======================== GLOBAL TAGS ===============

float tag1_1200;
unsigned long  tag2_1200;
int tag3_1200;
byte tag4_1200;


float tag1_200;
unsigned long  tag2_200;
int tag3_200;
byte tag4_200;
// ===================== GLOBAL TAGS =================


//====================FUNCTION 1 ====================//
void WizReset() {
  Serial.print("Resetting Wiz W5500 Ethernet Board...  ");
  pinMode(RESET_P, OUTPUT);
  digitalWrite(RESET_P, HIGH);
  delay(250);
  digitalWrite(RESET_P, LOW);
  delay(50);
  digitalWrite(RESET_P, HIGH);
  delay(350);
  Serial.println("Done from WIzreset.");
}
//====================FUNCTION 1 ====================//


//============================FUNCTION 3 ============================//
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

//============================FUNCTION 2 ============================//
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
//============================FUNCTION 2 ============================//





//********************************************
//======================= TASK 1 S7 200
//********************************************

void Task1code( void * pvParameters ) {
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    xSemaphoreTake( mate_200, portMAX_DELAY );
    Serial.println("IN 200 task Task 1");
    /////////////**********************READING DATA ****************************//////////////////////
    int Size, Read_200, Write_200;
   
    byte MyBuffer[1024];
    byte MyWrite[4];

    Serial.print("Reading ===s7 200===");

    //MarkTime();

    Read_200 = Client_200.ReadArea(S7AreaDB, // We are requesting DB access
                                   1,        // DB Number = 1
                                   0,        // Start from byte N.0
                                   11,     // We need "Size" bytes  Stricly put the bytes that has created in S7 DB if 11 there then put here only 11
                                   &MyBuffer);  // Put them into our target (Buffer or PDU)

    Serial.print("REsult >>>>>>> 200");
    Serial.println(Read_200);
    if (Read_200 == 0)
    {
      //ShowTime();
      tag1_200  = S7.FloatAt(&MyBuffer, 0);
      Serial.println("tag1_200");
      Serial.println(tag1_200);

      tag2_200 = S7.DWordAt(&MyBuffer, 4);
      Serial.println("tag2_200");
      Serial.println(tag2_200);

      S7.SetDWordAt(&MyWrite, 0, tag2_200);
      //Serial.println(MyWrite);
      Write_200 = Client_1200.WriteArea(S7AreaDB, // We are requesting DB access
                                   1,        // DB Number = 1
                                   4,        // Start from byte N.0
                                   4,     // We need "Size" bytes
                                   &MyWrite);  // Put them into our target (Buffer or PDU)

      tag3_200 = S7.IntegerAt(&MyBuffer, 8);
      Serial.println("tag3_200");
      Serial.println(tag3_200);

      tag4_1200  = S7.ByteAt(&MyBuffer, 10);
      Serial.println("tag4_1200");
      Serial.println(tag4_1200);

      S7.SetByteAt(&MyWrite, 0, tag4_1200);
      //Serial.println(MyWrite);
      Write_200 = Client_1200.WriteArea(S7AreaDB,  // We are requesting DB access
                                   1,            // DB Number = 1
                                   10,           // Start from byte N.0
                                   1,          // We need "Size" bytes Stricly put the bytes that has created in S7 DB if 11 there then put here only 11
                                   &MyWrite);  // Put them into our target (Buffer or PDU)

    }
    else {
      Serial.println("------------->>>>>>>> S7 200 ERROR <<<<<<<<<--------------");
      Size = 200;
      CheckError(Read_200, 200);
    }

    xSemaphoreGive(mate_200);
    vTaskDelay(1000 / portTICK_PERIOD_MS);   ///4000 ms data fetching 1200
  }
}


//*************======================*******************************
//======================= TASK 2 S7 1200
//*************======================*******************************

//Task2code: connecting the 1200
void Task2code( void * pvParameters ) {
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    xSemaphoreTake( mate_1200, portMAX_DELAY );
    Serial.println("IN 1200 task Task 2");
    /////////////**********************READING DATA ****************************//////////////////////
    int Size, Read_1200, Write_1200;
    
    byte MyBuffer[1024];
    byte MyWrite[4];

    Serial.print("Reading >>>>>>S7 1200 <<<<<<<");

    //MarkTime();

    Read_1200 = Client_1200.ReadArea(S7AreaDB, // We are requesting DB access
                                       1,        // DB Number = 1
                                       0,        // Start from byte N.0
                                       11,     // We need "Size" bytes  Stricly put the bytes that has created in S7 DB if 11 there then put here only 11
                                       &MyBuffer);  // Put them into our target (Buffer or PDU)
    Serial.print("REsult >>>>>>> 1200");
    Serial.println(Read_1200);
    if (Read_1200 == 0)
    {
      //ShowTime();
      tag1_1200 = S7.FloatAt(&MyBuffer, 0);
      Serial.println("tag1_1200");
      Serial.println(tag1_1200);

      S7.SetFloatAt(&MyWrite, 0, tag1_1200);
      //Serial.println(MyWrite);
      Write_1200 = Client_200.WriteArea(S7AreaDB, // We are requesting DB access
                                   1,        // DB Number = 1
                                   0,        // Start from byte N.0
                                   4,     // We need "Size" bytes  Stricly put the bytes that has created in S7 DB if 11 there then put here only 11
                                   &MyWrite);  // Put them into our target (Buffer or PDU)

      tag2_1200  = S7.DWordAt(&MyBuffer, 4);
      Serial.println("tag2_1200");
      Serial.println(tag2_1200);

      tag3_1200 = S7.IntegerAt(&MyBuffer, 8);
      Serial.println("tag3_1200");
      Serial.println(tag3_1200);

      S7.SetIntAt(&MyWrite, 0, tag3_1200);
      //Serial.println(MyWrite);
      Write_1200 = Client_200.WriteArea(S7AreaDB, // We are requesting DB access
                                   1,        // DB Number = 1
                                   8,        // Start from byte N.0
                                   2,     // We need "Size" bytes  Stricly put the bytes that has created in S7 DB if 11 there then put here only 11
                                   &MyWrite);  // Put them into our target (Buffer or PDU)

      tag4_1200 = S7.ByteAt(&MyBuffer, 10);
      Serial.println("tag4_1200");
      Serial.println(tag4_1200);

    }
    else {
      Serial.println("------------->>>>>>>> S7 1200 ERROR <<<<<<<<<--------------");
      Size = 1200;
      CheckError(Read_1200, 1200);
    }

    xSemaphoreGive(mate_1200);
    vTaskDelay(1500 / portTICK_PERIOD_MS);   ///4000 ms data fetching 1200
  }
}

//*************======================*******************************
//======================= TASK 3 S7 connection
//*************======================*******************************
//Task3code: Checking Connection with S7

void Task3code( void * pvParameters ) {
  Serial.print("Task3 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    //Serial.print("Task 3 running");
    // Connection for PLC S7 1200
    while (!Client_1200.Connected)
    {
      xSemaphoreTake(mate_1200, portMAX_DELAY);
      if (!Connect_1200()) {
        Serial.print("----------- S7 >> 1200 IN LOOP NOT ABLE TO CONNECT << ----------");
        delay(500);
      }
      else {
        xSemaphoreGive( mate_1200 );
      }
    }
    // Connection for PLC S7 200
    while (!Client_200.Connected && connected_1200 == 1200)
    {
      xSemaphoreTake(mate_200, portMAX_DELAY);
      if (!Connect_200()) {
        Serial.print("----------- S7 === 200 IN LOOP NOT ABLE TO CONNECT << ----------");
        delay(500);
      }
      else {
        xSemaphoreGive( mate_200 );
      }

    }

    vTaskDelay(10 / portTICK_PERIOD_MS);   ///10 ms data fetching
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
  Ethernet.begin(eth_MAC, eth_IP, eth_DNS, eth_GW, eth_MASK);

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
    /*while (true) {
        delay(10);          // Halt.
      }*/
  } else {
    Serial.println(" OK");
  }

  mate_1200 = xSemaphoreCreateMutex();

  mate_200 = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(
    Task1code,   /* Task function. */
    "Task1",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task1,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */


  //<<<<<<<<<<<<<<<<<<<<<________________TASK 2 _________________>>>>>>>>>>>>>>

  xTaskCreatePinnedToCore(
    Task2code,   /* Task function. */
    "Task2",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task2,      /* Task handle to keep track of created task */
    1);          /* pin task to core 0 */


  //<<<<<<<<<<<<<<<<<<<<<________________TASK 3 _________________>>>>>>>>>>>>>>

  xTaskCreatePinnedToCore(
    Task3code,   /* Task function. */
    "Task3",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    2,           /* priority of the task */
    &Task3,      /* Task handle to keep track of created task */
    1);         /* pin task to core 1 */
}
////////////////////////////////END SETUP //////////////////////////////////


//----------------------------------------------------------------------
// Connects to the PLC S7 1200
//----------------------------------------------------------------------
bool Connect_1200() {
  int Result_1200 = Client_1200.ConnectTo(S7_1215,
                                          0,  // Rack (see the doc.)
                                          1); // Slot (see the doc.)
  Serial.print("------============ CONNECTING TO S7 1200================---------- "); Serial.println(S7_1215);
  if (Result_1200 == 0)
  {
    connected_1200 = 1200;
    Serial.print(" -------  S7 1200  Connected !  -------  ");
    //Serial.print(" -------  S7 1200  Connected ! PDU Length = "); Serial.println(Client_1200.GetPDULength());
  }
  else {
    Serial.println(" ************** Connection error  S7 1200 ************** ");


    Ethernet.init(5);         // SS pin
    WizReset();
    //Serial.println("Starting ETHERNET connection...");
    Ethernet.begin(eth_MAC, eth_IP, eth_DNS, eth_GW, eth_MASK);

    //delay(2000);              // give the Ethernet shield a second to initialize

    vTaskDelay(2000 / portTICK_PERIOD_MS);   ///1000 ms data fetching

    Serial.print("Ethernet IP is: ");
    Serial.println(Ethernet.localIP());

    return Result_1200 == 0;
  }
}



//----------------------------------------------------------------------
// Connects to the PLC S7 1200
//----------------------------------------------------------------------
bool Connect_200()
{
  int Result_200 = Client_200.ConnectTo(S7_200,
                                        0,  // Rack (see the doc.)
                                        1); // Slot (see the doc.)
  Serial.print("------============ CONNECTING TO S7 200================---------- "); Serial.println(S7_200);
  if (Result_200 == 0)
  {
    Serial.print(" -------  S7 200  Connected ! -------  ");
    //Serial.print(" -------  S7 200  Connected ! PDU Length = "); Serial.println(Client_200.GetPDULength());
  }
  else {
    Serial.println("**************** Connection Error  S7 200 ****************");
    Ethernet.init(5);         // SS pin
    WizReset();
    //Serial.println("Starting ETHERNET connection...");
    Ethernet.begin(eth_MAC, eth_IP, eth_DNS, eth_GW, eth_MASK);
    //delay(2000);              // give the Ethernet shield a second to initialize

    vTaskDelay(2000 / portTICK_PERIOD_MS);   ///1000 ms data fetching
    Serial.print("Ethernet IP is: ");
    Serial.println(Ethernet.localIP());

    return Result_200 == 0;
  }
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
void loop() {

  delay(1000);
}


//----------------------------------------------------------------------
// Prints the Error number
//----------------------------------------------------------------------
void CheckError(int ErrNo, int interlock) {
  Serial.print("Error No. 0x");
  Serial.println(ErrNo, HEX);

  // Checks if it's a Severe Error => we need to disconnect
  if (ErrNo & 0x00FF)
  {
    //Serial.println("S7 ERROR, connecting.");
    //Client.Disconnect();
    if (interlock == 1200) {
      Serial.println("S7 1200 AT CHECK ERROR.");
      Connect_1200();
    }
    else if (interlock == 200) {
      Serial.println("S7 200 AT CHECK ERROR.");
      Connect_200();
    }
  }
}
