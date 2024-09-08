/* Tesing modbus server on T-internet-com, board ESP32 Wrover module */

// Working okay. over ethernet and as a modbus server



//#if defined(LILYGO_T_INTER_COM)
#define ETH_TYPE ETH_PHY_LAN8720
#define ETH_ADDR 0
#define ETH_CLK_MODE ETH_CLOCK_GPIO0_OUT
#define ETH_RESET_PIN 4
#define ETH_MDC_PIN 23
#define ETH_MDIO_PIN 18
#define SD_MISO_PIN 2
#define SD_MOSI_PIN 15
#define SD_SCLK_PIN 14
#define SD_CS_PIN 13
#define MODEM_RX_PIN 35
#define MODEM_TX_PIN 33
#define MODEM_PWRKEY_PIN 32
#define RGBLED_PIN 12

#include <Arduino.h>
#if ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 0, 0)
#include <ETHClass2.h>  //Is to use the modified ETHClass
#define ETH ETH2
#else
#include <ETH.h>
#endif
#include <SPI.h>
#include <SD.h>
//#include "utilities.h"          //Board PinMap


// Modbus Lib
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else  //ESP32
#include <WiFi.h>
#endif
#include <ModbusIP_ESP8266.h>

const int TEST_HREG = 100;
int inc = 1535;

//ModbusIP object
ModbusIP mb;

//Change to IP and DNS corresponding to your network, gateway
IPAddress staticIP(192, 168, 36, 244);
IPAddress gateway(192, 168, 36, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 36, 1);

static bool eth_connected = false;

void WiFiEvent(arduino_event_id_t event) {
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      Serial.println("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-ethernet");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.print("ETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      eth_connected = true;
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      break;
    default:
      break;
  }
}



void setup() {
  Serial.begin(115200);
  WiFi.onEvent(WiFiEvent);

#ifdef ETH_POWER_PIN
  pinMode(ETH_POWER_PIN, OUTPUT);
  digitalWrite(ETH_POWER_PIN, HIGH);
#endif

#if CONFIG_IDF_TARGET_ESP32
  if (!ETH.begin(ETH_TYPE, ETH_ADDR, ETH_MDC_PIN,
                 ETH_MDIO_PIN, ETH_RESET_PIN, ETH_CLK_MODE)) {
    Serial.println("ETH start Failed!");
  }
#else
  if (!ETH.begin(ETH_PHY_W5500, 1, ETH_CS_PIN, ETH_INT_PIN, ETH_RST_PIN,
                 SPI3_HOST,
                 ETH_SCLK_PIN, ETH_MISO_PIN, ETH_MOSI_PIN)) {
    Serial.println("ETH start Failed!");
  }
#endif

  if (ETH.config(staticIP, gateway, subnet, dns, dns) == false) {
    Serial.println("Configuration failed.");
  }

  mb.server();
  //mb.addHreg(TEST_HREG, 0xABCD);
  mb.addHreg(100, inc, 2);
}



void loop() {
  // put your main code here, to run repeatedly:

  // if (eth_connected) {
  //   Serial.print("Hi Pranav connected to ethernet");
  if (inc < 20000) {
    inc = inc + 1;
  } else {
    inc = 0;
  }
  mb.Hreg(100, inc);

  mb.task();
  Serial.println(inc);

  delay(50);
  //delay(10000);
}
