
#include "LoRaWANNode.h"
#include <stdio.h>
#include <stdlib.h>

#define PUSHBUTTON  PC13
#define FRAME_DELAY_BY_TIME    true  // Sending method (TIME or PUSH BUTTON)       
#define FRAME_DELAY         5000      // Time between 2 frames
#define DATA_RATE           4
#define ADAPTATIVE_DR       true
#define ENABLE              1
#define DISABLE             0
#define  CONFIRMED          false
#define PORT                1

HardwareSerial SerialLora(D0, D1); // D0(Rx) D1(TX)
HardwareSerial Serial1(PA10, PA9);

const char appKey[] = "E24F43FFFE44CE1D7C96EF9AA9DF9ED8";
const char appEUI[] = "70B3D57ED0017552";

char frameTx[] = "Hello";
String str;

void setup()
{
  Serial1.begin(115200);
  pinMode(PUSHBUTTON, INPUT);
  infoBeforeActivation();
  Serial1.println(" JOIN procedure in progress ...");  
  
  while(!loraNode.joinOTAA(appKey, appEUI)) {
    Serial1.println(" JOIN OTAA failed!!! Retry...");
    delay(1000);
  }
  
  Serial1.println(" JOIN procedure : SUCCESS !\r\n");
  infoAfterActivation();  
}

void loop()
{
  if( FRAME_DELAY_BY_TIME == 0)   while(digitalRead(PUSHBUTTON)); // Attente Push Button pour envoyer
  else                            delay(FRAME_DELAY);             // Attente FRAME_DELAY pour envoyer
  Serial1.print(" Sending Text : \"");Serial1.print(frameTx);Serial1.print("\"");
  if(CONFIRMED)   Serial1.print(" Uplink CONFIRMED on PORT ");
  else            Serial1.print(" Uplink UNCONFIRMED on PORT ");
  Serial1.println(PORT);
  transmit();
  receive();
}

void receive(void) {
  uint8_t frameRx[64];
  uint8_t len;
  uint8_t port;

  // Check if data received from a gateway
  if(loraNode.receiveFrame(frameRx, &len, &port)) {
    uint8_t n = 0;
    Serial1.print(" Frame received: 0x");
    while(len > 0) {
      Serial1.print(frameRx[n], HEX);
      Serial1.print(',');
      len--;
      n++;
    }
    Serial1.print(" on port "); Serial1.println(port);Serial1.print("\r\n");
  } else {
    Serial1.println(" No data received\r\n");
  }
}

void transmit(void) {
  // Send unconfirmed data to a gateway (port 1 by default)
  int status = loraNode.sendFrame(frameTx, sizeof(frameTx), CONFIRMED);
  if(status == LORA_SEND_ERROR) {
    Serial1.println(" Send Frame failed!!!");
  } else if(status == LORA_SEND_DELAYED) {
    Serial1.println(" Module is busy : \r\n * It's still trying to send data \r\n OR * \r\n * You are over your allowed duty cycle");
  } else {
   Serial1.println(" Frame sent");
  }
}

void infoBeforeActivation(void){
  Serial1.println("\r\n\r\n\r\n");
  Serial1.println("########################################");
  Serial1.println("######## LoRaWAN Training Session ######");
  Serial1.println("#########     OTAA activation   ########\r\n");

  while(!loraNode.begin(&SerialLora, LORA_BAND_EU_868)) {
    Serial1.println(" Lora module not ready");
    delay(1000);
  }
  
 
  str = " * Device EUI :      0x ";
  loraNode.getDevEUI(&str);
  Serial1.println(str);
  str = " * Application key : 0x ";
  loraNode.getAppKey(&str);
  Serial1.println(str);
  str = " * Application EUI : 0x ";
  loraNode.getAppEUI(&str);
  Serial1.println(str);Serial1.print("\r\n");

  loraNode.setAdaptativeDataRate(DISABLE);
  loraNode.setDataRate(DATA_RATE);
  Serial1.print(" * Data Rate : ");Serial1.print(loraNode.getDataRate());Serial1.print("\r\n");
  if(ADAPTATIVE_DR) {
    loraNode.setAdaptativeDataRate(ENABLE);
    Serial1.print(" * Adaptative Data Rate : ON");Serial1.println("\r\n");
  }
  else {
    Serial1.print(" * Adaptative Data Rate : OFF");Serial1.println("\r\n");
  }       
  loraNode.setDutyCycle(DISABLE);


}

void infoAfterActivation(void){
  str = " * Network session Key:     0x ";
  loraNode.getNwkSKey(&str);
  Serial1.println(str);

  str = " * Application session key: 0x ";
  loraNode.getAppSKey(&str);
  Serial1.println(str);

  str = " * Device address:          0x ";
  loraNode.getDevAddr(&str);
  Serial1.println(str);Serial1.print("\r\n");

  if(FRAME_DELAY_BY_TIME == 1){
    Serial1.print(" Frame will be sent every");Serial1.print(FRAME_DELAY);Serial1.println("\r\n");
  }
  else {
    Serial1.println(" Press Blue Button to send a Frame\r\n");
  }
}
