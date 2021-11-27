
// Select your modem:
//#include <SoftwareSerial.h>
//SoftwareSerial Sim800l(2,4);
#define TINY_GSM_MODEM_SIM800 // Modem is SIM800L
//#define Serial2 Serial2
// Define the serial console for debug prints, if needed
//#define TINY_GSM_DEBUG SerialMons

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
//const char apn[] = "internet.ng.airtel.com";
const char apn[] = "airtelgprs.com";
const char gprsUser[] = "";
const char gprsPass[] = "";

// SIM card PIN (leave empty, if not defined)
const char simPIN[]   = "";

// MQTT details

const char* broker = "165.22.216.124";                    // Public IP address or domain name
const char* mqttUsername = "";  // MQTT username
const char* mqttPassword = "";  // MQTT password

const char* topicOutput1 = "esp/tapan";

const char* topicTemperature = "esp/temp";

// Define the serial console for debug prints, if needed
//#define DUMP_AT_COMMANDS


#include <TinyGsmClient.h>
#include <PubSubClient.h>

TinyGsm modem(Serial2);
TinyGsmClient client(modem);

PubSubClient mqtt(client);



uint32_t lastReconnectAttempt = 0;


float temperature = 0;
float humidity = 0;
long lastMsg = 0;


void mqttCallback(char* topic, byte* message, unsigned int length) {
  Serial.print(". Message: ");
  char messageTemp[100];

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp[i] = (char)message[i];
    Serial.print(messageTemp);
  }
  Serial.println();

  mqtt.publish(topicTemperature, messageTemp);
  if (String(topic) == "esp/tapan") {
    // Serial.print("Changing output to ");
    if (messageTemp == "true") {
      Serial.println("true");
    }
    else if (messageTemp == "false") {
      Serial.println("false");
    }
  }

}


boolean mqttConnect() {
  Serial.print("Connecting to ");
  Serial.print(broker);

  // Connect to MQTT Broker without username and password
  //boolean status = mqtt.connect("GsmClientN");

  // Or, if you want to authenticate MQTT:
  boolean status = mqtt.connect("GsmClientN", mqttUsername, mqttPassword);

  if (status == false) {
    Serial.println(" fail");
    ESP.restart();
    return false;
  }
  Serial.println(" success");
  mqtt.subscribe(topicOutput1);
  return mqtt.connected();
}


void setup() {
  // Set console baud rate
  Serial.begin(115200);
  delay(10000);

  Serial.println("Wait...");

  // Set GSM module baud rate and UART pins
  Serial2.begin(9600);
  delay(6000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  modem.restart();
  delay(10000);

  modem.init();

  String modemInfo = modem.getModemInfo();
  Serial.print("Modem Info: ");
  Serial.println(modemInfo);

  // Unlock your SIM card with a PIN if needed
  if ( GSM_PIN && modem.getSimStatus() != 3 ) {
    modem.simUnlock(GSM_PIN);
  }

  String smsMessage = "Hello from ESP32!";
  String SMS_TARGET = "+919428468921";

  /*if (modem.sendSMS(SMS_TARGET, smsMessage)) {
    Serial.println(smsMessage);
  }
  else {
    Serial.println("SMS failed to send");
  }*/

  Serial.print("Connecting to APN: ");
  Serial.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    Serial.println(" fail");
    ESP.restart();
  }
  else {
    Serial.println(" OK");
  }

  if (modem.isGprsConnected()) {
    Serial.println("GPRS connected");
  }

  // MQTT Broker setup
  mqtt.setServer(broker, 1883);
  mqtt.setCallback(mqttCallback);
}

void loop() {
  
  if (!mqtt.connected()) {
    Serial.println("=== MQTT NOT CONNECTED ===");
    // Reconnect every 10 seconds
    uint32_t t = millis();
    if (t - lastReconnectAttempt > 10000L) {
      lastReconnectAttempt = t;
      if (mqttConnect()) {
        lastReconnectAttempt = 0;
      }
    }
    delay(100);
    return;
  }

  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;

    Serial.print("Temperature: ");
    Serial.println("Hi Pranav");
    mqtt.publish(topicTemperature, "online");
  }

  mqtt.loop();
}
