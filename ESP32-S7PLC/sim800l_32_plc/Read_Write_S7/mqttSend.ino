void mqttSend() {

  String interlock = plc_out;
  //Serial.print(interlock);
  if (interlock == "{}") {
    //Serial.println("----------KSU JSE NHI SANTI RAKHO ------------");
  } else {
    boolean rc = mqtt.publish("esp/pub", plc_out);
    if (rc == true) {
      Serial.println("-----------------MQTT Sent--------------");
    } else {

      Serial.println("------------->>>>>>>> MQTT Not able to send from Task 1 <<<<<<<<<--------------");
    }
  }
}
