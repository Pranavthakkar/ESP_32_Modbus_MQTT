void loop() {
  // put your main code here, to run repeatedly:

 if (!mqtt.connected()) {
    Serial.print("MQTT CLIENT NOT ABLE TO CONNECT");
    reconnect();
  }


 while (!Client.Connected) {
    if (!Connect())
      Serial.print("----------- S7 CLIENT NOT ABLE TO CONNECT ----------");
    delay(500);
  }

  while (Client.Connected) {


    long now = millis();
    if (now - lastMsg > 2000) {
      lastMsg = now;
    s7Read();
    mqttSend();
    }
  }

  mqtt.loop();
}
