/* Modbus TCP Server
 *  Getting Multiple Holding Register value   ** ONLY READING
 *  LeN 100 here
 *  All working Correctly     MQTT IS NOT DONE YET
 *  
 *  reading from 0 to LEN
 *  
 *  27-8-2021 12:25PM
*/

#include <SPI.h>
#include <Ethernet.h>       // Ethernet library v2 is required
#include <ModbusEthernet.h>

#define LEN 10      // LENGTH OF MODBUS REGISTER

#define RESET_P 26        // Tie the Wiz820io/W5500 reset pin to ESP32 GPIO26 pin.
// Enter a MAC address and IP address for your controller below.
uint8_t eth_MAC[] = { 0x02, 0xF0, 0x0D, 0xBE, 0xEF, 0x01 };

IPAddress remote(10,0,0,178);    // Address of Modbus Slave device

IPAddress eth_IP(10, 0, 0, 178);    // *** CHANGE THIS to something relevant for YOUR LAN. ***
IPAddress eth_MASK(255, 255, 255, 0);   // Subnet mask.
IPAddress eth_DNS(8, 8, 8, 8);    // *** CHANGE THIS to match YOUR DNS server.           ***
IPAddress eth_GW(10, 0, 0, 1);   // *** CHANGE THIS to match YOUR Gateway (router).     ***

const int COUNT = 2;                    // Count of Coils
uint16_t res[COUNT] = {78,79};



ModbusEthernet mb;              // Declare ModbusTCP instance

// Callback function to read corresponding DI
uint16_t cbRead(TRegister* reg, uint16_t val) {
  Serial.print("Read. Reg RAW#: ");
  Serial.print(reg->address.address);
  Serial.print(" Old: ");
  Serial.print(reg->value);
  Serial.print(" New: ");
  Serial.println(val);
  return val;
}

bool cb(Modbus::ResultCode event, uint16_t transactionId, void* data) { // Modbus Transaction callback
  if (event != Modbus::EX_SUCCESS)                  // If transaction got an error
    Serial.printf("Modbus result: %02X\n", event);  // Display Modbus error code
  if (event == Modbus::EX_TIMEOUT) {    // If Transaction timeout took place
    mb.disconnect(remote);              // Close connection to slave and
    mb.dropTransactions();              // Cancel all waiting transactions
  }
  return true;
}

// Callback function to write-protect DI
uint16_t cbWrite(TRegister* reg, uint16_t val) {
  Serial.print("Write. Reg RAW#: ");
  Serial.print(reg->address.address);
  Serial.print(" Old: ");
  Serial.print(reg->value);
  Serial.print(" New: ");
  Serial.println(val);
  return val;
}


// Callback function for client connect. Returns true to allow connection.
bool cbConn(IPAddress ip) {
  Serial.println(ip);
  return true;
}

void WizReset() {
    Serial.print("Resetting Wiz W5500 Ethernet Board...  ");
    pinMode(RESET_P, OUTPUT);
    digitalWrite(RESET_P, HIGH);
    delay(250);
    digitalWrite(RESET_P, LOW);
    delay(50);
    digitalWrite(RESET_P, HIGH);
    delay(350);
    Serial.println("Done.");
}

void prt_hwval(uint8_t refval) {
    switch (refval) {
    case 0:
        Serial.println("No hardware detected.");
        break;
    case 1:
        Serial.println("WizNet W5100 detected.");
        break;
    case 2:
        Serial.println("WizNet W5200 detected.");
        break;
    case 3:
        Serial.println("WizNet W5500 detected.");
        break;
    default:
        Serial.println
            ("UNKNOWN - Update espnow_gw.ino to match Ethernet.h");
    }
}

void prt_ethval(uint8_t refval) {
    switch (refval) {
    case 0:
        Serial.println("Uknown status.");
        break;
    case 1:
        Serial.println("Link flagged as UP.");
        break;
    case 2:
        Serial.println("Link flagged as DOWN. Check cable connection.");
        break;
    default:
        Serial.println
            ("UNKNOWN - Update espnow_gw.ino to match Ethernet.h");
    }
}

void setup() {
  Serial.begin(115200);     // Open serial communications and wait for port to open
  
  Ethernet.init(5);        // SS pin
  WizReset();

  Serial.println("Starting ETHERNET connection...");
  //Ethernet.begin(mac, ip);  // start the Ethernet connection
  Ethernet.begin(eth_MAC, eth_IP, eth_DNS, eth_GW, eth_MASK);

  delay(2000);              // give the Ethernet shield a second to initialize
  
  Serial.print("Ethernet IP is: ");
  Serial.println(Ethernet.localIP());

    /*
     * Sanity checks for W5500 and cable connection.
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
        Serial.println
            ("\n\r\tHardware fault, or cable problem... cannot continue.");
        Serial.print("Hardware Status: ");
        prt_hwval(Ethernet.hardwareStatus());
        Serial.print("   Cable Status: ");
        prt_ethval(Ethernet.linkStatus());
        while (true) {
            delay(10);          // Halt.
        }
    } else {
        Serial.println(" OK");
    }
    
  mb.onConnect(cbConn);   // Add callback on connection event
  mb.server();              // Act as Modbus TCP server
  

  if (!mb.addHreg(0, 0xF0F0, LEN)) Serial.println("Error"); // Add Hregs
  mb.onGetHreg(0, cbRead, LEN); // Add callback on Coils value get
  //mb.onSetHreg(0, cbWrite, LEN);
}

void loop() {
    mb.task(); // Modbus task
    delay(1000); // Pushing interval
}
