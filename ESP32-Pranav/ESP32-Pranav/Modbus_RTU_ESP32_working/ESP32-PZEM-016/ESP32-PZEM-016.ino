 
/*
 * Basic routine (Simple) Modbus RTU Protocol (Master), Created for communication with PeaceFair Meters PZEM-016 (Modbus RS485)  *
 * Rutina basica (Sencilla) Protocolo Modbus RTU (Maestro), Creada para comunicacion con Medidores PZEM-016 (Modbus RS485) de PeaceFair * 
 *  Created by PDAControl
 *   
 *   
 *  More info      : https://pdacontrolen.com/basic-modbus-rtu-master-rs485-test-with-esp32-xy-k485-pzem-016-without-libraries/
 *  Mas informacion: http://pdacontroles.com/prueba-basica-modbus-rtu-maestro-rs485-con-esp32-xy-k485-pzem-016-sin-librerias/
 *  Video: Tutorial Complete : https://youtu.be/FwKbfq1WYSM
 *  
 *  
 *  Channel  Youtube https://www.youtube.com/c/JhonValenciaPDAcontrol/videos   
 *  
 * Reading Routine 10 Registers (Input Registers) 0x04
 * It is a basic routine, it is not a complete implementation.
 * Requires ESP32 + Converter (RS485-TTL) XY-K485 + PZEM-016 Meter (Modbus RTU RS485)
 *  
 * Rutina de Lectura 10 Registros (Input Registers) 0x04
 * Es una rutina basica, no es una implementacion completa.
 * Requiere ESP32 + Convertidor (RS485-TTL) XY-K485 + Medidor PZEM-016(Modbus RTU RS485)
 *  
 *  
*/
 
#define RXD2 16
#define TXD2 17

byte ByteArray[250];
int ByteData[20];
 
void setup() {

  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);  /// Speed default PZEM-016 Modbus
}
 
void loop() {

 delay(200);


  //// -  1 step
 ///// Master frame request 10 registers (Input Registers) ///////////////////////////////////////////
 ///// Trama de Maestro solicitud 10 registros (Input Registers) ////////////////////////////////////
       
 byte msg[] = {0x01,0x04,0x00,0x00,0x00,0x0A,0x70,0x0D};

 int i;
 int len=8; 

////// Sending Frame Modbus for Serial Port 2 
/////// Envio de Trama Modbus por Puerto Serial #2

Serial.println("ENVIO DATOS  -  SEND DATA");
for(i = 0 ; i < len ; i++){
      Serial2.write(msg[i]); 
      Serial.print("[");
      Serial.print(i);
      Serial.print("]");
      Serial.print("=");   
      Serial.print(String(msg[i], HEX));      
 }
 len = 0;
 Serial.println();
 Serial.println();
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////





//// -  2 step
////////// Reception  Frame 10 Registers (Input Registers) Modbus RTU - RS485 ////////////////////////
/////////// Recepción de Trama 10 Registros (Input Registers) Modbus RTU - RS485 //////////////////////

int a = 0;
 while(Serial2.available()) 
 {
 ByteArray[a] = Serial2.read();
 a++;
 }

 int b = 0;
 String registros;
    Serial.println("DATA RECEPTION  -  RECEPCIÓN DATOS");
    for(b = 0 ; b < a ; b++){      
      Serial.print("[");
      Serial.print(b);
      Serial.print("]");
      Serial.print("=");    
    
      registros =String(ByteArray[b], HEX);   
      Serial.print(registros);
      Serial.print(" ");         
   }
       Serial.println();
       Serial.println();
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////



//// -  3 step
//////// Procesamiento de Registros HEX  //////////////////////////////////////////////////////////
//////// HEX Registers Processing ////////////////////////////////////// ////////////////////
//////// Conversion de 2 Byte a 1 int - Conversion from 2 Byte to 1 int

Serial.println("REGISTERS HEX");

ByteData[0] = ByteArray[3] * 256 + ByteArray[4];
Serial.println(ByteData[0],DEC);
ByteData[1] = ByteArray[5] * 256 + ByteArray[6];
Serial.println(ByteData[1],DEC);
//ByteData[2] = ByteArray[7] * 256 + ByteArray[8];
//Serial.println(ByteData[2],DEC);
ByteData[3] = ByteArray[9] * 256 + ByteArray[10];
Serial.println(ByteData[3],DEC);
//ByteData[4] = ByteArray[11] * 256 + ByteArray[12];
//Serial.println(ByteData[4],DEC);
ByteData[5] = ByteArray[13] * 256 + ByteArray[14];
Serial.println(ByteData[5],DEC);
//ByteData[6] = ByteArray[15] * 256 + ByteArray[16];
//Serial.println(ByteData[6],DEC);
ByteData[7] = ByteArray[17] * 256 + ByteArray[18];
Serial.println(ByteData[7],DEC);
ByteData[8] = ByteArray[19] * 256 + ByteArray[20];
Serial.println(ByteData[8],DEC);
//ByteData[9] = ByteArray[21] * 256 + ByteArray[22];
//Serial.println(ByteData[9],DEC);
//ByteData[10] = ByteArray[23] * 256 + ByteArray[24];
//Serial.println(ByteData[10],DEC);

Serial.println();
Serial.println();
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////



//// -  4 step
///////// Securities Normalization ////////////////////////////////////// /////////////////////////
/////////Normalizacion de Valores//////////////////////////////////////////////////////////////////

float Voltage,Current,Power,Energy,Frequency,PowerFactor;
Voltage     = ByteData[0] * 0.1;     // Tensión (80-260VAC)
Current     = ByteData[1] * 0.001;   // Corriente (0-100A)
Power       = ByteData[3] * 0.1;     // Potencia Activa (0-23000W)
Energy      = ByteData[5] ;          // Potencia Acumulada (0-9999kWh)
Frequency   = ByteData[7] * 0.1;     // Frecuencia (45-65Hz)
PowerFactor = ByteData[8] * 0.01;    // Factor de Potencia (0.00 – 1.00)  

Serial.println("MEDICIONES - MEASUREMENTS");
Serial.print("Voltage ");
Serial.println(Voltage);
Serial.print("Current ");
Serial.println(Current);
Serial.print("Power ");
Serial.println(Power);
Serial.print("Energy ");
Serial.println(Energy);
Serial.print("Frequency ");
Serial.println(Frequency);
Serial.print("PowerFactor ");
Serial.println(PowerFactor);
Serial.println();
delay(200);                      /// delay para permitir ver valores - delay to allow viewing values -
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


   
}
