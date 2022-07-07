void s7Read(){

  MarkTime();

    Result = Client.ReadArea(S7AreaDB,    // We are requesting DB access
                             3,           // DB Number = 1
                             206,         // Start from byte N.0
                             8,           // We need "Size" bytes
                             &MyBuffer);  // Put them into our target (Buffer or PDU)
    if (Result == 0) {
      ShowTime();

      //---------------------making an object -----------------//

      StaticJsonDocument<64> current_value;

      StaticJsonDocument<150> element;
      // convert it to a JsonObject
      JsonObject root = element.to<JsonObject>();

      current_value["t1"] = S7.IntegerAt(&MyBuffer, 0);

      current_value["t2"] = S7.IntegerAt(&MyBuffer, 2);

      current_value["t3"] = S7.IntegerAt(&MyBuffer, 4);

      current_value["t4"] = S7.IntegerAt(&MyBuffer, 6);

      Serial.println("Previous Value::::::");
      serializeJsonPretty(prev_value, Serial);  //Printing the JSON Payload
      Serial.println("Current Value::::::");
      serializeJsonPretty(current_value, Serial);  //Printing the JSON Payload


    //==================== localRBE ==================//
      for (int i = 1; i <= 4; i++) {

        String p = String(i);
        String j = "t" + p;

        if (current_value[j] != prev_value[j]) {
          root[j] = current_value[j];
          prev_value[j] = current_value[j];
        }
      }

    //==================== localRBE ==================//
    
      Serial.println("Root Value::::::");
      serializeJsonPretty(root, Serial);  //Printing the JSON Payload
      serializeJson(root, plc_out);

      Serial.println("---------------- S7 COMMUNICITON DONE ---------------");

    } 
    
    else {
      Serial.println("------------->>>>>>>> S7 Not Comminication <<<<<<<<<--------------");
      CheckError(Result);
    }

}