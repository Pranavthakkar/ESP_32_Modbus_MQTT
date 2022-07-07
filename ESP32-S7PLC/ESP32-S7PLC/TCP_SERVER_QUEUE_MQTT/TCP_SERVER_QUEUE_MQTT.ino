/*
 * Getting data on mqtt and sending in to the first 3 modbus registers.
 * https://industruino.com/blog/our-news-1/post/modbus-tips-for-industruino-26  for float to byte conv   
 * TCP READ AND WRITE ARE SENDING ON MQTT 
 * 
 * QUEUE USING HERE FOR STORING TCP SERVER DATA AND SEND IT BACK TO THE MQTT. 
 * @Pranav 3 SEPT 2021 - JUST STARTED
*/

#include <SPI.h>
#include <Ethernet.h>       // Ethernet library v2 is required
#include <ModbusEthernet.h>
//#include "Settimino.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>


#define RESET_P 26        // Tie the Wiz820io/W5500 reset pin to ESP32 GPIO26 pin.
#define LEN 10            // LENGTH OF MODBUS REGISTER

TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;
TaskHandle_t Task4;

QueueHandle_t queue;

//SemaphoreHandle_t baton;

//Modbus Registers Offsets
const int base = 100;
// Enter a MAC address and IP address for your controller below.
uint8_t eth_MAC[] = { 0x02, 0xF0, 0x0D, 0xBE, 0xEF, 0x01 };

IPAddress remote(192, 168, 1, 178);    // *** CHANGE THIS to something relevant for YOUR LAN. ***

IPAddress eth_IP(192, 168, 1, 178);    // *** CHANGE THIS to something relevant for YOUR LAN. ***
IPAddress eth_MASK(255, 255, 255, 0);   // Subnet mask.
IPAddress eth_DNS(8, 8, 8, 8);    // *** CHANGE THIS to match YOUR DNS server.           ***
IPAddress eth_GW(192, 168, 1, 1);   // *** CHANGE THIS to match YOUR Gateway (router).     ***

//==========================MQTT =====================
const char* mqtt_server = "165.22.216.124";

EthernetClient espClient;
PubSubClient client(espClient);
String messageTemp;
const char* publish_topic = "esp32/tcp";
const char* sub_topic = "esp32/output";

// tag name starting
String tag_name = "t";
//==========================MQTT =======================


//==================== Global variable for localRBE=================
StaticJsonDocument<192> prev_value;       // recommanded to use https://arduinojson.org/v6/assistant/
char plc_out[300];
uint16_t values[10] ;
// ==================== Global variable for localRBE=================

ModbusEthernet  mb;     // Modbus communication


// Callback function to read corresponding hR

uint16_t cbRead(TRegister* reg, uint16_t val) {
  /*Serial.print("Read. Reg RAW#: ");
    reg_num = reg->address.address;

    Serial.println(reg_num);
    Serial.println(reg->value);
    Serial.println(reg->address.address);
    Serial.print(" Old: ");
    Serial.print(reg->value);
    Serial.print(" New: ");
    Serial.println(val);
  */
  return val;
}


/*
  uint16_t cbLed(TRegister* reg, uint16_t val) {          // was using this before  Used for if any value change in server get that value cbLed = cbWrite
  Attach ledPin to LED_COIL register
  Serial.println(reg->address.address);
  Serial.print(" New: ");
  Serial.println(val);
  return val;
  }*/


// BELOW NOT NEEDED
/*bool cb(Modbus::ResultCode event, uint16_t transactionId, void* data) { // Modbus Transaction callback
  if (event != Modbus::EX_SUCCESS)                  // If transaction got an error
    Serial.printf("Modbus result: %02X\n", event);  // Display Modbus error code
  if (event == Modbus::EX_TIMEOUT) {    // If Transaction timeout took place
    mb.disconnect(remote);              // Close connection to slave and
    mb.dropTransactions();              // Cancel all waiting transactions
  }
  return true;
  }*/




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

// For float to int_16 conversion
unsigned int f_2uint_int1(float float_number) {             // split the float and return first unsigned integer
  union f_2uint {
    float f;
    uint16_t i[2];
  };

  union f_2uint f_number;
  f_number.f = float_number;

  return f_number.i[0];
}

// For float to int_16 conversion
unsigned int f_2uint_int2(float float_number) {            // split the float and return first unsigned integer

  union f_2uint {
    float f;
    uint16_t i[2];
  };

  union f_2uint f_number;
  f_number.f = float_number;

  return f_number.i[1];
}


//Task1code: FROM MQTT TO TCP SERVER
void Task1code( void * pvParameters ) {
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {

    //---  Parse the mqtt payload and send it to modbus tcp

    StaticJsonDocument<1024> mqttData;
    deserializeJson(mqttData, messageTemp);   //print the mqtt data in json format
    JsonObject obj = mqttData.as<JsonObject>();
    //serializeJsonPretty(obj, Serial);   //Printing the JSON Payload

    if (obj != 0) {

      serializeJsonPretty(obj, Serial);   //Printing the JSON Payload

      for (uint8_t t = 1; t <= 10; t++) {
        String p = String(t);
        String local = tag_name + p;

        if (obj.containsKey(local)) {     // Check if json has that key

          //if (mb.Hreg(t, 70)) messageTemp = "";
          mb.Hreg(0, t);        // to the first 40000 register
          mb.Hreg(1, f_2uint_int2(obj[local]));     // to the Third 40001 register
          mb.Hreg(2, f_2uint_int1(obj[local]));     // to the second 40002 register


          /** Note Here You can use mb.addHreg for adding and assiginig its value no need to worry about existing register
            you can add this register beyond len of existing register
            mb.addHreg(20, f_2uint_int2(obj[local]));     // to the Third 40001 register
            mb.addHreg(21, f_2uint_int1(obj[local]));     // to the second 40002 register **/


          messageTemp = "";
          delay(1000);
        }
      }
    }
    vTaskDelay(1500 / portTICK_PERIOD_MS);
  }
}


//Task2code: Calling the modbus task function here
void Task2code( void * pvParameters ) {
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {

    mb.task();

    vTaskDelay(100 / portTICK_PERIOD_MS);   ///100 ms data fetching
  }
}


//Task3code: READING MODBUS REGISTERS VALUE AND SET AS LOCAL RBE and send it to mqtt
void Task3code( void * pvParameters ) {
  Serial.print("Task3 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {

    xSemaphoreTake( baton, portMAX_DELAY );

    StaticJsonDocument<192> current_value;

    StaticJsonDocument<192> element;

    JsonObject root = element.to<JsonObject>();

    //Serial.println("Current Value::::::");
    //serializeJsonPretty(current_value, Serial);   //Printing the JSON Payload
    //Serial.println("GETTING STARRT PLC OUT");
    //Serial.print(plc_out);

    //  uint16_t Hreg(uint16_t offset);
    for (int i = 0; i <= 10; i++) {         // here make sure that or not addHreg** that do not write on that 3 golden registers
      String p = String(i);
      String t = tag_name + p;

      current_value[t] = mb.Hreg(i);
    }

    for (int i = 0; i <= 10; i++) {
      //Serial.println("In For  LOOP");
      String p = String(i);
      String j = tag_name + p;

      if (current_value[j] != prev_value[j]) {
        root[j] = current_value[j];
        prev_value[j] = current_value[j];
      }
    }
    //Serial.println("Root Value::::::");
    //serializeJsonPretty(root, Serial);   //Printing the JSON Payload

    serializeJson(root, plc_out);

    xSemaphoreGive(baton);
    vTaskDelay(400 / portTICK_PERIOD_MS);   ///100 ms data fetching
  }
}


//Task4code: Sending MQTT MESSAGES
void Task4code( void * pvParameters ) {
  Serial.print("Task4 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {

    xSemaphoreTake( baton, portMAX_DELAY );

    String interlock = plc_out;
    //Serial.print(interlock);

    if (interlock == "{}") {
      //Serial.println("---------- ------------");
    }

    else {
      boolean rc = client.publish(publish_topic, plc_out);

      if (rc == true) {
        Serial.println("-----------------MQTT DONE--------------");
      } else {
        Serial.println("------------->>>>>>>> MQTT ERROR <<<<<<<<<--------------");
      }
    }

    xSemaphoreGive(baton);
    
    vTaskDelay(4000 / portTICK_PERIOD_MS);   ///100 ms data fetching
  }
}


//************* VOID SETUP *************
void setup() {

  for (int i = 0; i <= 10; i++) {
    String p = String(i);
    String t = tag_name + p;

    prev_value[t] = 0;
  }

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
      //delay(10);          // Halt. Do not halt here
    }
  } else {
    Serial.println(" OK");
  }

  mb.onConnect(cbConn);   // Add callback on connection event
  mb.server();

  mb.addHreg(0, 99, LEN);    //if you want to add register with starting value use this (base, starting value, length of registers)
  //mb.addReg(HREG(10));     // Add Holding register from #100 with out length

  //mb.onGetHreg(0, cbRead, LEN); // Add callback on Coils value get  // *** this will continously getting data over reading
  //mb.onSetHreg(0, cbLed, LEN); // Add callback on Coil LED_COIL value set  *** on set means if something is set from plc side it will gonna show here


  //==========================MQTT =====================
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  //==========================MQTT =====================


  baton = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(
    Task1code,   /* Task function. */
    "Task1",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task1,      /* Task handle to keep track of created task */
    1);          /* pin task to core 0 */


  //<<<<<<<<<<<<<<<<<<<<<________________TASK 2 _________________>>>>>>>>>>>>>>

  xTaskCreatePinnedToCore(
    Task2code,   /* Task function. */
    "Task2",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task2,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */


  //<<<<<<<<<<<<<<<<<<<<<________________TASK 3 _________________>>>>>>>>>>>>>>

  xTaskCreatePinnedToCore(
    Task3code,   /* Task function. */
    "Task3",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task3,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */

  //<<<<<<<<<<<<<<<<<<<<<________________TASK 3 _________________>>>>>>>>>>>>>>

  xTaskCreatePinnedToCore(
    Task4code,   /* Task function. */
    "Task4",     /* name of task. */
    1500,       /* Stack size of task */
    NULL,        /* parameter of the task */
    0,           /* priority of the task */
    &Task4,      /* Task handle to keep track of created task */
    1);          /* pin task to core 0 */
}
//************* VOID SETUP *************


//==========================MQTT CALLBACK===========================
void callback(char* topic, byte* message, unsigned int length) {

  //Serial.println("Message arrived on topic: ");
  //Serial.print(topic);
  //Serial.print(". Message: ");
  messageTemp = "";    // for getting new value every time so reseting it.

  for (int i = 0; i < length; i++) {
    //Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}
//==========================MQTT CALLBACK===========================


//==========================MQTT RECONNECT===========================
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("mqtt client connected");
      // Subscribe
      client.subscribe(sub_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
//==========================MQTT RECONNECT===========================



void loop() {
  //Call once inside loop() - all magic here
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  //mb.task();

  delay(1000);
}
