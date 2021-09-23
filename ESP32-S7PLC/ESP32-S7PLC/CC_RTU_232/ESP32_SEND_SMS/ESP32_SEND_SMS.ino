/*Running esp 32 with sim800L for sending SMS 
 * but the SIM is not registered yes blinking at rate of 1 sec

   19 sept 2021

   @Pranav
*/

//Include Libraries
#include <SPI.h>
#include <SoftwareSerial.h>

SoftwareSerial sim800l(16, 17); // (Rx,Tx  > Tx,Rx) // ESP32
char incomingByte;

String inputString;

//int relay = 2; // Output for Relay Control

char Received_SMS; //Here we store the full received SMS (with phone sending number and date/time) as char

String Data_SMS;  //Here's the SMS that we gonna send to the phone number, it may contain DHT data or MLX data



int interlock = 0;

char *myNumber[] = {
  "+916358758796",
  //"+919662293190",
  //"+918490003425"
};

//String f1001 = "+916358758796";


void setup()
{
  Serial.begin(9600);
  sim800l.begin(9600);

  //ReceiveMode();
}


void loop()
{

  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');

    if (command == "ON") {

      Serial.println("Pumping On");
      Data_SMS = "Pumping  on";
      sim800l.println("AT+CMGD=1");
      delay(500);
      //Send_Data();
      for (int i = 0; i < 1; i++) {
        sendsms(Data_SMS, myNumber[i]);
        Serial.println(myNumber[i]);
        delay(500);
      }
    }
  }

  /*if (interlock == 0) {

    }*/


  /*
    if(sim800l.available()){
    Serial.print("Inside IF");

    while(sim800l.available()){
    Serial.print("Inside while");

    incomingByte = sim800l.read();
    inputString += incomingByte;
    inputString.toUpperCase(); // Uppercase the Received Message
    }
    //pump_on = inputString.indexOf("ON");
    //pump_off = inputString.indexOf("OFF");

    ///////////////////////////////////// Getting to Know About Result //////////////////
    Serial.println(inputString);

    ///////////////////////////////////// Getting to Know About Result //////////////////


    //        if(pump_on !=-1){
    //          Serial.println("Pumping On");
    //          Data_SMS="Your Pump is on";
    //
    //          Send_Data();
    //          ReceiveMode();
    //
    //          pump_on = -1;
    //          pump_off = -1;
    //        }
    //
    //        if(pump_off !=-1){
    //          delay(1000);
    //          Serial.println("Pumping Off");
    //          Data_SMS="Your Pump is Off";
    //
    //          Send_Data();
    //          ReceiveMode();
    //
    //          pump_on = -1;
    //          pump_off = -1;
    //        }


    /////////////////////////////Program Start ///////////////////////////////////


    /*
    if(inputString.indexOf("OFF") > -1){
    Serial.println("Pumping off");
    Data_SMS="Pumping off";
    Serial.println(inputString);

    const char text[] = "Pump1 Off";
    radio.write(&text, sizeof(text));

    sim800l.println("AT+CMGD=1");
    delay(500);
    for(int i=0; i<2;i++){
    sendsms(Data_SMS, myNumber[i]);
    Serial.println(myNumber[i]);
    delay(500);
    }
    }     */


}
///////////////////////////////////////Program END//////////////////////////////////////

/*
  void ReceiveMode() {      //Set the SIM800L Receive mode

  sim800l.println("AT"); //If everything is Okay it will show "OK" on the serial monitor
  Serialcom();
  sim800l.println("AT+CMGF=1"); // Configuring TEXT mode
  Serialcom();
  sim800l.println("AT+CNMI=2,2,0,0,0"); //Configure the SIM800L on how to manage the Received SMS... Check the SIM800L AT commands manual
  Serialcom();
  sim800l.println("AT+CMGR=1"); //Configure the SIM800L on how to manage the Received SMS... Check the SIM800L AT commands manual
  Serialcom();
  }*/



/*
  void Serialcom() //This is used with ReceiveMode function, it's okay to use for tests with Serial monitor
  {
  delay(500);
  while (Serial.available())
  {
    sim800l.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while (sim800l.available())
  {
    Serial.write(sim800l.read());//Forward what Software Serial received to Serial Port
  }
  }*/


//void Send_Data()
//{
//  Serial.println("Sending Data...");     //Displays on the serial monitor...Optional
//  sim800l.print("AT+CMGF=1\r");          //Set the module to SMS mode
//  delay(100);
//  sim800l.print("AT+CMGS=\"+916358758796\"\r");  //Your phone number don't forget to include your country code example +212xxxxxxxxx"

//  delay(500);
//  sim800l.print(Data_SMS);  //This string is sent as SMS
//  delay(500);
//  sim800l.print((char)26);//Required to tell the module that it can send the SMS
//  delay(500);
//  sim800l.println();
//  Serial.println("Data Sent.");
//  delay(500);
//  //sim800l.println("AT+CMGDA=\"DEL ALL\"\r");
//  //delay(1000);
//Serial.println("Data Sent.");
//  delay(500);
//
//}

void sendsms(String message, String number)
{
  String mnumber = "AT+CMGS=\"" + number + "\"\r";
  Serial.println("Sending Data...");
  sim800l.print("AT+CMGF=1\r");
  delay(200);
  sim800l.print(mnumber);  // recipient's mobile number, in international format
  delay(500);
  Serial.println(mnumber);
  sim800l.print(message);                         // message to send
  delay(500);
  sim800l.print((char)26);                        // End AT command with a ^Z, ASCII code 26
  delay(500);

  sim800l.println();
  delay(500);                                     // give module time to send SMS
  Serial.println("Data Sent.");
  delay(2000);
}
