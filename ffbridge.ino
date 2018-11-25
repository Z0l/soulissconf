#include "SoulissFramework.h"
#include "bconf/StandardArduino.h"
#include "conf/ethW5100.h"
#include "conf/usart.h"
#include "conf/SuperNode.h"

#define USARTDRIVER_INSKETCH
#define USARTBAUDRATE_INSKETCH
#define USART_BAUD9k6           1
#define USART_TXENABLE          1
#define USART_TXENPIN           2
#define USARTDRIVER             Serial1
#define SERIALPORT_INSKETCH
#define LOG          Serial
#define VNET_DEBUG_INSKETCH
#define VNET_DEBUG  1

#include <SPI.h>

/*** All configuration includes should be above this line ***/
#include "Souliss.h"

//define IP network config
uint8_t ip_address[4]  = {192, 168, 1, 78};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};
#define myvNet_address  ip_address[3]
#define gw_address 77

// Define the RS485 network configuration
#define myvNet_subnet 0xFF00
#define Emelet_RS485 0xCD01
#define GLent 0xCD02
#define GFent 0xCD03
#define Tekla 0xCD04
#define Halo 0xCD05
#define Vendeg 0xCD06
#define Furdo 0xCD07

// Local light slots
#define SL_MUTEREM 0
#define SL_TEKLA 1
#define SL_HALO 2
#define SL_VENDEG 3
#define SL_FURDO 4

// Local blind slots
#define SB_TEKLA 5
#define SB_HALO 6

// Local light output PINs
#define MUTEREM 22
#define TEKLA 23
#define HALO 24
#define VENDEG 25
#define FURDO 26

// Local blind output PINs
#define TEKLAF 28
#define TEKLAL 29
#define HALOF 30
#define HALOL 31

void setup() {
  
  Initialize();
  Serial.begin(9600);  //needed for debug only
  
  Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
  SetAddress(Emelet_RS485, myvNet_subnet, 0x0000);                    // Set the address on the RS485 bus
  SetAddress(myvNet_address, myvNet_subnet, gw_address);

    // Define PINs as output
  for (int thisPin = 22; thisPin < 32; thisPin++) {
      pinMode(thisPin, OUTPUT);  
  }

  // Drive lights low by default
  for (int thisPin = 22; thisPin < 27; thisPin++) {
      digitalWrite(thisPin, LOW);  
  }
  
  // Drive motors high by default
  for (int thisPin = 28; thisPin < 32; thisPin++) {
      digitalWrite(thisPin, HIGH);  
  }

  // Define light logic
  Set_T11(SL_MUTEREM);
  Set_T11(SL_TEKLA);
  Set_T11(SL_HALO);
  Set_T11(SL_VENDEG);
  Set_T11(SL_FURDO);  

  // Define T22 logic for all motors
  Set_T22(SB_TEKLA);
  Set_T22(SB_HALO);
  
}

void loop() {
    EXECUTEFAST() {
    UPDATEFAST();

    FAST_110ms() {
      // This starts the lights logic
      Logic_T11(SL_MUTEREM);
      Logic_T11(SL_TEKLA);
      Logic_T11(SL_HALO);
      Logic_T11(SL_VENDEG);
      Logic_T11(SL_FURDO);
      
      // This starts the blind logic
      Logic_T22(SB_TEKLA);
      Logic_T22(SB_HALO);

      //This pulls the PINs High for light control
      DigOut(MUTEREM, Souliss_T1n_Coil, SL_MUTEREM);
      DigOut(TEKLA, Souliss_T1n_Coil, SL_TEKLA);
      DigOut(HALO, Souliss_T1n_Coil, SL_HALO);
      DigOut(VENDEG, Souliss_T1n_Coil, SL_VENDEG);
      DigOut(FURDO, Souliss_T1n_Coil, SL_FURDO);
      
      // This pulls the PINs Low for blind control
      LowDigOut(TEKLAF, Souliss_T2n_Coil_Open, SB_TEKLA);
      LowDigOut(TEKLAL, Souliss_T2n_Coil_Close, SB_TEKLA);
      LowDigOut(HALOF, Souliss_T2n_Coil_Open, SB_HALO);
      LowDigOut(HALOL, Souliss_T2n_Coil_Close, SB_HALO);
    }

    FAST_BridgeComms();

  }

}
