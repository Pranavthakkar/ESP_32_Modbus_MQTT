


//==========================MQTT =====================
const char* broker = "165.22.216.124";  // Public IP address or domain name
const char* mqttUsername = "";          // MQTT username
const char* mqttPassword = "";          // MQTT password

const char* topicSub = "esp/sub";
const char* topicPub = "esp/pub";

//==========================MQTT ======================


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
}


//==========================MQTT ===============================
void reconnect() {
  Serial.print("Connecting to ");
  Serial.print(broker);

  // Loop until we're reconnected
  while (!mqtt.connected()) {

    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (mqtt.connect("helloEs32")) {
      
      mqtt.subscribe(topicSub);
      Serial.println("MQTT connected");

    }

    else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}