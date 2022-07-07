void s7Read() {

  MarkTime();


  Result = Client.ReadArea(S7AreaDB,    // We are requesting DB access
                           dbNumber,           // DB Number = 1
                           0,         // Start from byte N.0
                           maxSize,           // We need "Size" bytes
                           &MyBuffer);  // Put them into our target (Buffer or PDU)*/

  if (Result == 0) {

    ShowTime();

    //---------------------making an object -----------------//

    StaticJsonDocument<4096> current_value;
    //StaticJsonDocument<60> element;
    DynamicJsonDocument element(4096);

    // convert it to a JsonObject
    JsonObject root = element.to<JsonObject>();


    for (int t = 0; t <= maxTags; t++) {

      String p = String(t);
      String local = startingString + p ;
      //Serial.println(local);
      int offset_value = myArr[t].value;
      //Serial.println(offset_value);

      if (checkReal(local) == true) {
        current_value[local] = S7.FloatAt(&MyBuffer, offset_value);
      }
      else if (checkInt(local) == true) {
        current_value[local] = S7.IntegerAt(&MyBuffer, offset_value);
      }

      else if (checkDInt(local) == true) {
        current_value[local] = S7.DintAt(&MyBuffer, offset_value);
      }

      else {
        //Serial.println("I'm in Nothing");
      }
    }

    //Serial.print("------------------------- Previous Value ------------------------- ");
    //serializeJsonPretty(prev_value, Serial);  //Printing the JSON Payload

    //Serial.print("-------------------------  Current Value ------------------------- ");
    //serializeJsonPretty(current_value, Serial);  //Printing the JSON Payload


    //==================== localRBE ==================//
    for (int i = 0; i < maxTags; i++) {
      String j = "T" + String(i);

      if (current_value[j] != prev_value[j]) {
        root[j] = current_value[j];
        prev_value[j] = current_value[j];
      }
    }
    //==================== localRBE ==================//

    Serial.print("------------------------- Root Value ------------------------- ");
    serializeJsonPretty(root, Serial);  //Printing the JSON Payload
    serializeJson(root, plc_out); 

    //Serial.println("---------------- S7 COMMUNICITON DONE ---------------");
  }

  else {
    Serial.println("------------->>>>>>>> S7 Not Comminication <<<<<<<<<--------------");
    CheckError(Result);
  }

}
