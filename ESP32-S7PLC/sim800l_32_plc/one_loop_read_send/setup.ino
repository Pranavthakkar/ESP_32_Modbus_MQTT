void setup() {

  // Open serial communications and wait for port to open:
  Serial.begin(115200);

  prev_value["t1"] = 0;
  prev_value["t2"] = 0;
  prev_value["t3"] = 0;
  prev_value["t4"] = 0;


  connectEthernet();

  sim800l();

  //==========================MQTT =====================
  mqtt.setServer(broker, 1883);
  mqtt.setCallback(callback);

  mqtt.setKeepAlive(60);
  mqtt.setSocketTimeout(60);
  //==========================MQTT =====================

}
////////////////////////////////END SETUP //////////////////////////////////
