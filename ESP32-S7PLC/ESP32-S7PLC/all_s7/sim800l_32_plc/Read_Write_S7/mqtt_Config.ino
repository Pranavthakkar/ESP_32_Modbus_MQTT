

//==========================MQTT =====================
const char* broker = "165.22.216.124";  // Public IP address or domain name
const char* mqttUsername = "";          // MQTT username
const char* mqttPassword = "";          // MQTT password
//==========================MQTT ======================



void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  messageTemp = "";    // for getting new value every time so reseting it.
  //String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  s7_write(messageTemp);

}



//==========================MQTT ============================

void reconnect() {
  Serial.print("Connecting to ");
  Serial.print(broker);

  // Loop until we're reconnected
  while (!mqtt.connected()) {

    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (mqtt.connect("ppad")) {
      Serial.println("MQTT connected");
      mqtt.subscribe("esp32/pranav");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      ESP.restart();
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
