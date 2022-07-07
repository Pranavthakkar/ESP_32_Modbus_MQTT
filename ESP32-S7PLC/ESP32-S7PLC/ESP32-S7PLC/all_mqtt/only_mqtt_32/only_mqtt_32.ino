/* WRITING TO THE PLC S7 WRITE
 *  
 *  WITH MULTIPLE VALUES
 * ---  @PRANAV 24/8/2021  13:20


------------------------------------------------------------------------
----------------------------------------------------------------------*/
TaskHandle_t Task1;
TaskHandle_t Task2;

//#include "Platform.h"
#include "Settimino.h"
#include <Ethernet.h>
#include <SPI.h>



#define RESET_P 26        // Tie the Wiz820io/W5500 reset pin to ESP32 GPIO26 pin.
// Uncomment next line to perform small and fast data access
#define DO_IT_SMALL

//Mqtt 
#include <PubSubClient.h>
#include <ArduinoJson.h>


// The IP address will be dependent on your local network:
// Enter a MAC address and IP address for your controller below.

uint8_t eth_MAC[] = { 0x02, 0xF0, 0x0D, 0xBE, 0xEF, 0x01 };


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
//byte mac[] = { 
//  0x90, 0xA2, 0xDA, 0x0F, 0x08, 0xE1 };

IPAddress PLC(10,0,0,180);   // PLC Address


//W5500 LAN
IPAddress eth_IP(10, 0, 0, 178);    // *** CHANGE THIS to something relevant for YOUR LAN. ***
IPAddress eth_MASK(255, 255, 255, 0);   // Subnet mask.
IPAddress eth_DNS(8, 8, 8, 8);    // *** CHANGE THIS to match YOUR DNS server.           ***
IPAddress eth_GW(10, 0, 0, 1);   // *** CHANGE THIS to match YOUR Gateway (router).     ***



//==========================MQTT =====================
const char* mqtt_server = "165.22.216.124";
EthernetClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;
String messageTemp;
//==========================MQTT ======================

// =================== S7 COMMUNICATION ===============
/*int Size, Result;
float Pressure;
unsigned long Encoder;
int Component;
byte Bytes;
byte MyBuffer[1024];
byte MyWrite[4];
S7Client Client;

unsigned long Elapsed; // To calc the execution time

typedef struct { 
  String key;
  uint8_t  value;
} offsetDict;

const offsetDict myArr[] {
    {"T1",0},{"T2",4},{"T3",8},{"T4",10}
};*/
// =================== S7 COMMUNICATION ===============


//==================== Global variable for localRBE=================
StaticJsonDocument<64> prev_value;
char plc_out[150];

// ==================== Global variable for localRBE=================


// ==================== FUNCTION 1 ==================== //
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
//==================== FUNCTION 1 ==================== //


//==================== FUNCTION 3 ====================//
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

//==================== FUNCTION 2 ==================== //
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
// ==================== FUNCTION 2 ==================== //



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
        /*while (true) {
            delay(10);          // Halt.
        }*/
    } else {
        Serial.println(" OK");
    }

//<<<<<<<<<<<<<<<<<<<<<________________TASK 1 _________________>>>>>>>>>>>>>>
    xTaskCreatePinnedToCore(
                      mqtt,   /* Task function. */
                      "Task1",     /* name of task. */
                      10000,       /* Stack size of task */
                      NULL,        /* parameter of the task */
                      1,           /* priority of the task */
                      &Task1,      /* Task handle to keep track of created task */
                      0);          /* pin task to core 0 */                  


//<<<<<<<<<<<<<<<<<<<<<________________TASK 2 _________________>>>>>>>>>>>>>>

    xTaskCreatePinnedToCore(
                          s7write,   /* Task function. */
                          "Task2",     /* name of task. */
                          10000,       /* Stack size of task */
                          NULL,        /* parameter of the task */
                          1,           /* priority of the task */
                          &Task2,      /* Task handle to keep track of created task */
                          1);          /* pin task to core 0 */                  

 
  //==================== MQTT =====================
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
  //==================== MQTT =====================
}
//==================== END SETUP ==================== //




//Task1code: SENDING the MQTT
void mqtt( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    //For small client is used for mqtt
   
    
  /*StaticJsonDocument<300> doc;

    doc["sensor"] = "gps";
    doc["time"] = 1313123;

    //JsonArray data = doc.createNestedArray("data");
    
    //data["Encoder"] = Encoder;
    //data["Component"] = Component;
    //data["Bytes"] = Bytes;
    JsonObject obj = doc.createNestedObject("data");
    obj["Pressure"] = Pressure;
    obj["Encoder"] = Encoder;
    obj["Component"] = Component;
    obj["Bytes"] = Bytes;
    
   
    //serializeJsonPretty(doc, Serial);   //Printing the JSON Payload

    char out[256];
      int b = serializeJson(doc,out);
      //Serial.print("bytes = ");
      //Serial.println(b,DEC);*/
    
    //String interlock = plc_out;
    //Serial.print(interlock);
    
    /*if(interlock == "{}"){
      Serial.println("----------KSU JSE NHI SANTI RAKHO ------------");
    }
    else{
      boolean rc = client.publish("esp32/S7", plc_out);
      
      if(rc == true){
      Serial.println("-----------------MQTT DONE--------------");
      } else {
      Serial.println("------------->>>>>>>> MQTT ERROR <<<<<<<<<--------------");
      }
    }*/
    
    StaticJsonDocument<2048> qweqw;
    
    deserializeJson(qweqw, messageTemp);
    
    JsonObject obj = qweqw.as<JsonObject>();

    serializeJsonPretty(obj, Serial);   //Printing the JSON Payload
    Serial.println("----------IN mqtt-------------");
    Serial.println(messageTemp);
    /*float pp = obj[String("T1")];
    Serial.print("Printing the t1 value");
    Serial.println(pp);*/

    
    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}



//Task2code: connecting S7 COMMUNICATION write
void s7write( void * pvParameters ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

    for(;;){
    
    vTaskDelay(500 / portTICK_PERIOD_MS);   ///100 ms data fetching
    }
}


//================ MQTT CALLBACK ================
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  messageTemp = "";

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
   
  Serial.println();

  
  if (String(topic) == "esp32/mqtt") {
    Serial.println("================================================");
  }
}

//==========================MQTT ===============================
void reconnect(){
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("esp32pranav")) {
      Serial.println("MQTT connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
//==========================MQTT ============================

//----------------------------------------------------------------------
// Connects to the PLC
//----------------------------------------------------------------------
/*bool Connect()
{
    int Result=Client.ConnectTo(PLC, 
                                  0,  // Rack (see the doc.)
                                  1); // Slot (see the doc.)
    Serial.print("------ S7  Connecting to ");Serial.println(PLC);  
    if (Result==0) 
    {
      Serial.print(" -------  S7  Connected ! PDU Length = ");Serial.println(Client.GetPDULength());
    }
    else{
      Serial.println("Connection error");

      
      Ethernet.init(5);         // SS pin
      WizReset();
      Serial.println("Starting ETHERNET connection...");
      Ethernet.begin(eth_MAC, eth_IP, eth_DNS, eth_GW, eth_MASK);
  
      delay(2000);              // give the Ethernet shield a second to initialize
      
      Serial.print("Ethernet IP is: ");
      Serial.println(Ethernet.localIP());
      
      return Result==0;
    }
}*/



//----------------------------------------------------------------------
// Main Loop
//----------------------------------------------------------------------
void loop() 
{
   if (!client.connected()) {
    Serial.print("MQTT CLIENT NOT ABLE TO CONNECT");
    reconnect();
    }
    client.loop();
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
    Serial.println("S7 ERROR, connecting.");
    //Client.Disconnect(); 
    //Connect();
  }
}
