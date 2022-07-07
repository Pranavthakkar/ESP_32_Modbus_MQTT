
uint8_t eth_MAC[] = { 0x02, 0xF0, 0x0D, 0xBE, 0xEF, 0x01 };



IPAddress PLC(172, 17, 1, 1);  // PLC Address

IPAddress eth_IP(172, 17, 1, 18);      // IP address of your esp32 ***
IPAddress eth_MASK(255, 255, 255, 0);  // Subnet mask.
IPAddress eth_DNS(8, 8, 8, 8);         // *** CHANGE THIS to match YOUR DNS server.           ***
IPAddress eth_GW(172, 17, 1, 1);       // *** CHANGE THIS to match YOUR Gateway (router).     ***

/*// Following constants are needed if you are connecting via WIFI
  // The ssid is the name of my WIFI network (the password obviously is wrong)
  char ssid[] = "";    // Your network SSID (name)
  char pass[] = "";  // Your network password (if any)
  IPAddress Gateway(171, 17, 1, 1);
  IPAddress Subnet(255, 255, 255, 0);*/



void connectEthernet() {

  Ethernet.init(5);  // SS pin
  WizReset();

  Serial.println("Starting ETHERNET connection...");
  Ethernet.begin(eth_MAC, eth_IP, eth_DNS, eth_GW, eth_MASK);

  delay(1000);  // give the Ethernet shield a second to initialize
  Serial.print("Ethernet IP is: ");
  Serial.println(Ethernet.localIP());

  /*
       Sanity checks for W5500 and cable connection.
  */

  Serial.print("Checking connection.");

  bool rdy_flag = false;
  for (uint8_t i = 0; i <= 20; i++) {
    if ((Ethernet.hardwareStatus() == EthernetNoHardware)
        || (Ethernet.linkStatus() == LinkOFF)) {
      Serial.print(".");
      rdy_flag = false;
      delay(80);
    } else {
      rdy_flag = true;
      break;
    }
  }

  if (rdy_flag == false) {
    Serial.println("\n\r\tHardware fault, or cable problem... cannot continue.");
    Serial.print("Hardware Status: ");
    prt_hwval(Ethernet.hardwareStatus());
    Serial.print("   Cable Status: ");
    prt_ethval(Ethernet.linkStatus());
    /*while (true) {
        delay(10);          // Halt.
      }*/
  } else {
    Serial.println(" OK");
  }

}

//----------------------------------------------------------------------
// Connects to the PLC
//----------------------------------------------------------------------
bool Connect() {
  int Result = Client.ConnectTo(PLC,
                                0,   // Rack (see the doc.)
                                1);  // Slot (see the doc.)
  Serial.print("------ S7  Connecting to ");
  Serial.println(PLC);
  if (Result == 0) {
    Serial.print(" -------  S7  Connected ! PDU Length = ");
    Serial.println(Client.GetPDULength());
  } else {
    Serial.println("Connection error");


    Ethernet.init(5);  // SS pin
    WizReset();
    Serial.println("Starting ETHERNET connection...");
    Ethernet.begin(eth_MAC, eth_IP, eth_DNS, eth_GW, eth_MASK);

    delay(2000);  // give the Ethernet shield a second to initialize

    Serial.print("Ethernet IP is: ");
    Serial.println(Ethernet.localIP());

    return Result == 0;
  }
}
