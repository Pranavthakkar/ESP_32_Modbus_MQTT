/* MQTT SENDING ONLY STRING WITH ETHERNET PORT CONNECTION DONE
 *  
 *  17:23 22-8-2021 
 *  YOU CAN CAHNGE THE HOST NAME OF MQTT SERVER
 *  

    

    Example: https://techtutorialsx.com/2017/04/29/esp32-sending-json-messages-over-mqtt/

    ArduinoJson Library : https://github.com/bblanchon/ArduinoJson

    working guide: http://www.steves-internet-guide.com/arduino-sending-receiving-json-mqtt/
*/


#include <SPI.h>
#include <Ethernet.h>

#include <PubSubClient.h>
//#include <ArduinoJson.h>


#define RESET_P 26        // Tie the Wiz820io/W5500 reset pin to ESP32 GPIO26 pin.

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
// Enter a MAC address and IP address for your controller below.

uint8_t eth_MAC[] = { 0x02, 0xF0, 0x0D, 0xBE, 0xEF, 0x01 };


IPAddress eth_IP(10, 0, 0, 178);    // *** CHANGE THIS to something relevant for YOUR LAN. ***
IPAddress eth_MASK(255, 255, 255, 0);   // Subnet mask.
IPAddress eth_DNS(8, 8, 8, 8);    // *** CHANGE THIS to match YOUR DNS server.           ***
IPAddress eth_GW(10, 0, 0, 1);   // *** CHANGE THIS to match YOUR Gateway (router).     ***
//==========================MQTT =====================
const char* mqtt_server = "165.22.216.124";

//EthernetClient
EthernetClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
//==========================MQTT ==================================================


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
  

  Ethernet.init(5);         // SS pin
  WizReset();

  Serial.println("Starting ETHERNET connection...");
  //Ethernet.begin(mac, ip);  // start the Ethernet connection
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
        while (true) {
            delay(10);          // Halt.
        }
    } else {
        Serial.println(" OK");
    }

  //==========================MQTT =====================
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  //==========================MQTT =====================

}

//==========================MQTT ==================================================


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
  if (String(topic) == "esp32/output") {
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

//==========================MQTT ==================================================

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
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

//==========================MQTT ==================================================



void loop() {
//<----------------------------MQTT CONNECTION ===============>>>
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
//<----------------------------MQTT CONNECTION ===============>>>
  
  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;

//    StaticJsonDocument<300> doc;
    /*JsonObject JSONencoder = JSONbuffer.createObject();

    JSONencoder["device"] = "ESP32";
    JSONencoder["sensorType"] = "Temperature";
    JsonArray& values = JSONencoder.createNestedArray("values");
    values.add(res[3]).c_str());
    values.add(21);

    char JSONmessageBuffer[100];
    JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer)); */

//    doc["sensor"] = "gps";
 //   doc["time"] = 1313123;

   /* JsonArray data = doc.createNestedArray("data");
    data.add(res[3]);
    data.add(213.993);

    //serializeJson(doc, Serial);
    //Serial.println();
    
    serializeJsonPretty(doc, Serial);

      char out[128];
      int b = serializeJson(doc,out);
      Serial.print("bytes = ");
      Serial.println(b,DEC);*/
          
    boolean rc = client.publish("esp32/1", "PRANAV");

    if(rc == true){
      Serial.println("Success sending message");
    } else {
      Serial.println("Error sending message");
    }
  }

}
