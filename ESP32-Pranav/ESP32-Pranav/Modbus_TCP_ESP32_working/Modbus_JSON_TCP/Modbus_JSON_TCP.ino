/* MQTT modbus trail with json done ESP32

    1 Aug 2021 20:29 PM trail success

    Example: https://techtutorialsx.com/2017/04/29/esp32-sending-json-messages-over-mqtt/

    ArduinoJson Library : https://github.com/bblanchon/ArduinoJson

    working guide: http://www.steves-internet-guide.com/arduino-sending-receiving-json-mqtt/
*/



#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <ModbusIP_ESP8266.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const int REG = 0;               // Modbus Hreg Offset
IPAddress remote(192, 168, 1, 103);  // Address of Modbus Slave device
const int LOOP_COUNT = 10;

ModbusIP mb;  //ModbusIP object


//==========================MQTT =====================
const char* mqtt_server = "165.22.216.124";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
//==========================MQTT ==================================================


void setup() {
  Serial.begin(115200);
  WiFi.begin("DIGIMECK ENGINEERS", "DIGI@2019");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //==========================MQTT =====================
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  //==========================MQTT =====================


  mb.client();
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

//==========================MQTT ==================================================


uint16_t res[10];
uint8_t show = LOOP_COUNT;




void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (mb.isConnected(remote)) {   // Check if connection to Modbus Slave is established
    mb.readHreg(remote, REG, res, 10); // Initiate Read Coil from Modbus Slave

    /*Serial.print("Register 3 :");
      Serial.println(res[3]);
      client.publish("esp32", String(res[3]).c_str());
    */

    //Serial.println(" ");
    //Serial.println(" ");

    //Serial.print("Register 4 :");
    //Serial.println(res[4]);

    //Serial.println(" ");
    //Serial.println(" ");

    //Serial.print("Register 5 :");
    //Serial.println(res[5]);

    //Serial.println(" ");
    //Serial.println(" ");
    //mb.readHreg(remote, REG, &res);  // Initiate Read Coil from Modbus Slave
  } else {
    mb.connect(remote);           // Try to connect if no connection
  }
  mb.task();                      // Common local Modbus task
  delay(1);                     // Pulling interval
  if (!show--) {                   // Display Slave register value one time per second (with default settings)
    show = LOOP_COUNT;
    //Serial.print("Show");
    //Serial.println(show);
  }

  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;

    /*Serial.print("Register 3 :");
      Serial.println(res[3]);
      client.publish("esp32", String(res[3]).c_str()); */

    StaticJsonDocument<300> doc;
    /*JsonObject JSONencoder = JSONbuffer.createObject();

    JSONencoder["device"] = "ESP32";
    JSONencoder["sensorType"] = "Temperature";
    JsonArray& values = JSONencoder.createNestedArray("values");
    values.add(res[3]).c_str());
    values.add(21);

    char JSONmessageBuffer[100];
    JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer)); */

    doc["sensor"] = "gps";
    doc["time"] = 1313123;

    JsonArray data = doc.createNestedArray("data");
    data.add(res[3]);
    data.add(213.993);

    //serializeJson(doc, Serial);
    //Serial.println();
    
    serializeJsonPretty(doc, Serial);

      char out[128];
      int b = serializeJson(doc,out);
      Serial.print("bytes = ");
      Serial.println(b,DEC);
          
    boolean rc = client.publish("esp32/1", out);

    if(rc == true){
      Serial.println("Success sending message");
    } else {
      Serial.println("Error sending message");
    }
  }

}
