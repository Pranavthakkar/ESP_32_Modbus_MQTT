/* freeRTOS with task 1 is mqtt on core 0 (1500 ms) and task 2 is s7 on core 1 (100 ms)
 *  Sending MQTT data over json format with freeRTOS task running.  
 *  
 *  Underdevelopment for diff of value to send on mqtt: ---- this is without localRBE 
 *  
 *  some time scan cycle is working properly
 * 
 * reading multiple DB from S7 is not done yet
 * 
 * ---  @PRANAV 22/8/2021  19:20


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


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
// Enter a MAC address and IP address for your controller below.

uint8_t eth_MAC[] = { 0x02, 0xF0, 0x0D, 0xBE, 0xEF, 0x01 };


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
//byte mac[] = { 
//  0x90, 0xA2, 0xDA, 0x0F, 0x08, 0xE1 };

IPAddress PLC(10,0,0,180);   // PLC Address

IPAddress eth_IP(10, 0, 0, 178);    // *** CHANGE THIS to something relevant for YOUR LAN. ***
IPAddress eth_MASK(255, 255, 255, 0);   // Subnet mask.
IPAddress eth_DNS(8, 8, 8, 8);    // *** CHANGE THIS to match YOUR DNS server.           ***
IPAddress eth_GW(10, 0, 0, 1);   // *** CHANGE THIS to match YOUR Gateway (router).     ***

/*// Following constants are needed if you are connecting via WIFI
// The ssid is the name of my WIFI network (the password obviously is wrong)
char ssid[] = "DIGIMECK ENGINEERS";    // Your network SSID (name)
char pass[] = "DIGI@2019";  // Your network password (if any)
IPAddress Gateway(171, 17, 1, 1);
IPAddress Subnet(255, 255, 255, 0);*/

//==========================MQTT =====================
const char* mqtt_server = "165.22.216.124";
EthernetClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;
//==========================MQTT ======================

int Size, Result;
float Pressure;
unsigned long Encoder;
int Component;
byte Bytes;
byte MyBuffer[1024];
byte MyWrite[4];

S7Client Client;

unsigned long Elapsed; // To calc the execution time

/////////////////////////FUNCTION 1 ////////////////
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
/////////////////////////FUNCTION 1 ////////////////


/////////////////////////FUNCTION 3 ///////////////////////////
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

/////////////////////////FUNCTION 2 ////////////////
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
/////////////////////////FUNCTION 2 ///////////////////////////



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

    //==========================MQTT =====================
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    //==========================MQTT =====================
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


}
////////////////////////////////END SETUP //////////////////////////////////




//Task1code: connecting the mqtt
void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    //For small client is used for mqtt
   
    
  StaticJsonDocument<300> doc;

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
      //Serial.println(b,DEC);
          
    boolean rc = client.publish("esp32/S7", out);

    if(rc == true){
      Serial.println("-----------------MQTT DONE--------------");
    } else {
      Serial.println("------------->>>>>>>> MQTT ERROR <<<<<<<<<--------------");
    }
    vTaskDelay(1500 / portTICK_PERIOD_MS);
  }
}




//Task2code: connecting the mqtt
void Task2code( void * pvParameters ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
  /////////////**********************READING DATA ****************************//////////////////////

  //Serial.print("Reading "); 

  MarkTime();
  
  Result=Client.ReadArea(S7AreaDB, // We are requesting DB access
                         1,        // DB Number = 1
                         0,        // Start from byte N.0
                         22,     // We need "Size" bytes
                         &MyBuffer);  // Put them into our target (Buffer or PDU)
  if (Result==0)
  {
    ShowTime();
    Pressure = S7.FloatAt(&MyBuffer, 0);
    //Serial.print("Pressure_0_index=");
    //Serial.println(Pressure); 
    Serial.println("---------------- S7 COMMUNICITON DONE ---------------"); 
    
    Encoder = S7.DWordAt(&MyBuffer, 4);
    //Serial.print("Encoder_4_index=");
    //Serial.println(Encoder);
    //Serial.println(""); 
    
    Component = S7.IntegerAt(&MyBuffer, 8);
    //Serial.print("Component_8_index=");
    //Serial.println(Component); 
    //Serial.println(""); 
    Bytes = S7.ByteAt(&MyBuffer, 10);
    //Serial.print("Bytes_10_index=");
    //Serial.println(Bytes);  
    //Serial.println(""); 
  }
  else{
    Serial.println("------------->>>>>>>> S7 ERROR <<<<<<<<<--------------");
    CheckError(Result);
  }
    
  vTaskDelay(100 / portTICK_PERIOD_MS);   ///100 ms data fetching
  }
}



void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == "esp32/asd") {
    Serial.print("Changing output to ");
    if (messageTemp == "on") {
      Serial.println("on");
      //digitalWrite(ledPin, HIGH);
    }
    else if (messageTemp == "off") {
      Serial.println("off");

    }
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
      //client.subscribe("esp32/output");
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
bool Connect()
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
}

//----------------------------------------------------------------------
// Profiling routines
//----------------------------------------------------------------------
void MarkTime()
{
  Elapsed=millis();
}

//----------------------------------------------------------------------
void ShowTime()
{
  // Calcs the time
  Elapsed=millis()-Elapsed;
  Serial.print("Job time (ms) : ");
  Serial.println(Elapsed);   
}
 


//----------------------------------------------------------------------
// Main Loop
//----------------------------------------------------------------------
void loop() 
{
//<<<<<<<<<<<<<<<<<<<<<-----------mqtt ------------------>>>>>>>>>>>>>>>>
   if (!client.connected()) {
    Serial.print("MQTT CLIENT NOT ABLE TO CONNECT");
    reconnect();
    }
    client.loop();
//<<<<<<<<<<<<<<<<<<<<<-----------mqtt ------------------>>>>>>>>>>>>>>>>

    
  // Connection for PLC
  while (!Client.Connected)
  {
    if (!Connect())
      Serial.print("----------- S7 CLIENT NOT ABLE TO CONNECT ----------");
      delay(500);
  }

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
    Connect();
  }
}
