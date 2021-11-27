//Task2code: connecting S7 COMMUNICATION write
/*void s7write( void * pvParameters ) {
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
/*
    //StaticJsonDocument<1024> mqttData;

    DynamicJsonDocument mqttData(1024);
    
    deserializeJson(mqttData, messageTemp);   //print the mqtt data in json format
    JsonObject obj = mqttData.as<JsonObject>();

    serializeJsonPretty(obj, Serial);   //Printing the JSON Payload



    if (obj != 0) {

      /////////////======================== WRITING DATA TO PLC =======================/////////////////////

      for (int t = 0; t < maxTags; t++) {        //Running the for getting all T1 - Tn

        //String p = String(t);
        String local = "T" + String(t);

        String dict_key = myArr[t].key;

        int dict_byte = myArr[t].value;



        if (obj.containsKey(local)) {     // Check if json has that key

          for (uint8_t r = 0; r < maxReal; r++) {          /// Always put the max value of String Array do not plus it

            //=============== FLOAT WORD ===============
            if (String(real[r]) == local) {

              float value = obj[local];
              byte MyWrite[4];

              S7.SetFloatAt(&MyWrite, 0, value);
              //Serial.println(MyWrite);
              Result = Client.WriteArea(S7AreaDB, // We are requesting DB access
                                        dbNumber,        // DB Number = 1
                                        dict_byte,        // Start from byte N.0
                                        4,     // We need "Size" bytes
                                        &MyWrite);  // Put them into our target (Buffer or PDU)
              //Serial.println("Result Float >>>>>>>>");
              //Serial.println(Result);

              if (Result == 0)
              {
                prev_value[local] = obj[local];
                messageTemp = "";    // PLC can also write so here i null the mqtt payload

                Serial.println("Send Successfully Float ");
                //Serial.println("-----------Real Success--------");
                //break;
              }
              else {
                Serial.println("ERROR RESULT FLOAT ");
              }
            }
          }

          //==================  DOUBLE WORD ============================
          for (uint8_t d = 0; d < maxDInt; d++) {


            if (String(doubleInt[d]) == local) {

              double value = obj[local];
              byte MyWrite[4];

              S7.SetDWordAt(&MyWrite, 0, value);

              Result = Client.WriteArea(S7AreaDB, // We are requesting DB access
                                        dbNumber,        // DB Number = 1
                                        dict_byte,        // Start from byte N.0
                                        4,     // We need "Size" bytes
                                        &MyWrite);  // Put them into our target (Buffer or PDU)
              //Serial.println("Result DWORD >>>>>>>>");
              //Serial.println(Result);

              if (Result == 0)
              {
                prev_value[local] = obj[local];
                messageTemp = "";    // PLC can also write so here i null the mqtt payload

                Serial.println("Send Successfully DWORD ");
                //Serial.println("-----------DWORD Success--------");
              }
              else {
                Serial.println("ERROR RESULT DworD ");
              }
            }
          }


          //==================  INTEGER ============================
          for (uint8_t rb = 0; rb < maxNumber; rb++) {

            if (number[rb] == local) {
              int value = obj[local];
              byte MyWrite[2];

              S7.SetIntAt(&MyWrite, 0, value);
              //Serial.println(MyWrite);
              Result = Client.WriteArea(S7AreaDB, // We are requesting DB access
                                        dbNumber,        // DB Number = 1
                                        dict_byte,        // Start from byte N.0
                                        2,     // We need "Size" bytes
                                        &MyWrite);  // Put them into our target (Buffer or PDU)
              //Serial.println("Result INTEGER >>>>>>>>");
              //Serial.println(Result);

              if (Result == 0)
              {
                prev_value[local] = obj[local];
                messageTemp = "";    // PLC can also write so here i null the mqtt payload

                Serial.println("Send Successfully INT ");
                //Serial.println("-----------INTEGER Success--------");
              }
              else {
                Serial.println("ERROR RESULT INT ");
              }
            }
          }


          /*   //==================  BYTES ============================
             for (uint8_t b = 0; b <= 4; b++) {
               if (byyte[b] == local) {
                 byte value = obj[local];
                 byte MyWrite[1];

                 S7.SetByteAt(&MyWrite, 0, value);
                 //Serial.println(MyWrite);
                 Result = Client.WriteArea(S7AreaDB, // We are requesting DB access
                                           1,        // DB Number = 1
                                           dict_byte,        // Start from byte N.0
                                           1,     // We need "Size" bytes
                                           &MyWrite);  // Put them into our target (Buffer or PDU)
                 Serial.println("Result BYTE >>>>>>>>");
                 Serial.println(Result);

                 if (Result == 0)
                 {
                   messageTemp = "";    // PLC can also write so here i null the mqtt payload
                   Serial.println("Send Successfully BYTE ");
                   Serial.println("-----------BYTE Success--------");
                 }
                 else {
                   Serial.println("ERROR RESULT BYTE");
                 }
               }
             }*/


 /*         //==================  BYTES ============================
        }
        else {

        }
      }
    }

    else {
      //Serial.println("NO MQTT DATA in S7 Write");
    }

    vTaskDelay(5000 / portTICK_PERIOD_MS);   ///100 ms data fetching
  }
}*/
