void loop() {
  // put your main code here, to run repeatedly:

  if (!mqtt.connected()) {
    Serial.print("MQTT CLIENT NOT ABLE TO CONNECT");
    reconnect();
  }

  mqtt.loop();

  while (!Client.Connected) {
    if (!Connect())
      Serial.print("----------- S7 CLIENT NOT ABLE TO CONNECT ----------");
    delay(500);
  }

  /*
    do {
      long now = millis();
      if (now - lastMsg > 5000) {
        lastMsg = now;
        s7Read();
        mqttSend();
      }
    }
    while (Client.Connected);*/

  long now = millis();
  if (now - lastMsg > 7000) {
    lastMsg = now;
    s7Read();
    mqttSend();
  }

  //s7Read();
  //mqttSend();
  //delay(10000);
  //s7_write();
  //delay(1000);
}

/*Pressure = S7.FloatAt(&MyBuffer, 6);
                Serial.print("Pressure_=");
                Serial.println(Pressure);

                tag2 = S7.IntegerAt(&MyBuffer,4);
                  Serial.print("Tag2=");
                  Serial.println(tag2);

                tag3 = S7.DintAt(&MyBuffer,0);
                  Serial.print("Tag3=");
                  Serial.println(tag3);*/
