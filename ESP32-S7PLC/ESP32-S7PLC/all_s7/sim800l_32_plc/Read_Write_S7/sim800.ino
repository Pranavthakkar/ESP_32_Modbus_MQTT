#define GSM_PIN ""

// Your GPRS credentials, if any
//const char apn[] = "internet.ng.airtel.com";
const char apn[] = "airtelgprs.com";
const char gprsUser[] = "";
const char gprsPass[] = "";

// SIM card PIN (leave empty, if not defined)
const char simPIN[] = "";


void sim800l() {


  //Sim800l
  Serial2.begin(115200);
  delay(100);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()

  Serial.println("Initializing modem...");

  modem.restart();

  delay(15000);

  modem.init();
  String modemInfo = modem.getModemInfo();

  Serial.print("Modem Info: ");
  Serial.println(modemInfo);

  // Unlock your SIM card with a PIN if needed
  if (GSM_PIN && modem.getSimStatus() != 3) {
    modem.simUnlock(GSM_PIN);
  }

  //String smsMessage = "Hello from ESP32!";
  //String SMS_TARGET = "+919428468921";

  //
  //  if (modem.sendSMS(SMS_TARGET, smsMessage)) {
  //    Serial.println(smsMessage);
  //  }
  //  else {
  //    Serial.println("SMS failed to send");
  //  }

  Serial.print("Connecting to APN: ");
  Serial.print(apn);

  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    Serial.println(" fail");
    ESP.restart();
  } else {
    Serial.println(" OK");
  }
  if (modem.isGprsConnected()) {
    Serial.println("GPRS connected");
  }

}
