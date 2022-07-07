void setup() {

  // Open serial communications and wait for port to open:
  Serial.begin(115200);

  for (int i = 0; i < maxTags; i++) {
    String t = startingString + String(i);
    prev_value[t] = 0;
  }
  //Serial.println(prev_value);
  //serializeJsonPretty(prev_value, Serial);  //Printing the JSON Payload
  connectEthernet();
  sim800l();

  //<<<<<<<<<<<<<<<<<<<<<________________TASK 1 _________________>>>>>>>>>>>>>>
 /* xTaskCreatePinnedToCore(
//    s7write,   /* Task function. */
//    "Task1",     /* name of task. */
//    10000,       /* Stack size of task */
//    NULL,        /* parameter of the task */
//    1,           /* priority of the task */
//    &Task1,      /* Task handle to keep track of created task */
//    0);          /* pin task to core 0 */
   

  //==========================MQTT =====================
  mqtt.setServer(broker, 1883);
  mqtt.setCallback(callback);

  //mqtt.setKeepAlive(60);
  mqtt.setBufferSize(4096);
  //mqtt.setSocketTimeout(60);*/
  //==========================MQTT =====================

}
////////////////////////////////END SETUP //////////////////////////////////
