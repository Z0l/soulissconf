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
/*#define SERIALPORT_INSKETCH
#define LOG          Serial
#define VNET_DEBUG_INSKETCH
#define VNET_DEBUG  1*/

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
#define myvNet_subnet   0xFF00
#define Gateway_RS485   0xDE01
#define GLent           0xDE02
#define GFent           0xDE03
#define Tekla           0xDE04
#define Halo            0xDE05
#define Vendeg          0xDE06
#define Furdo           0xDE07

// Local light slots
#define SL_MUTEREM 0
#define SL_TEKLA 1
#define SL_HALO 2
#define SL_VENDEG 3
#define SL_FURDO 4

// Local blind slots
#define SB_TEKLA 5
#define SB_HALO 6
#define BL_TIMEOUT 0xAD

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
  
  Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
  SetAddress(Gateway_RS485, myvNet_subnet, 0);                    // Set the address on the RS485 bus
  SetAddress(myvNet_address, myvNet_subnet, gw_address);
  
  //Serial.begin(9600);  //needed for debug only
    // Define PINs as output
  for (int thisPin = 22; thisPin < 32; thisPin++) {
      pinMode(thisPin, OUTPUT);  
  }

  // Drive lights low by default
  for (int thisPin = 22; thisPin < 32; thisPin++) {
      digitalWrite(thisPin, LOW);  
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
      Souliss_Logic_T22(memory_map, SB_TEKLA, &data_changed, BL_TIMEOUT);
      Souliss_Logic_T22(memory_map, SB_HALO, &data_changed, BL_TIMEOUT);

      //This pulls the PINs High for light control
      DigOut(MUTEREM, Souliss_T1n_Coil, SL_MUTEREM);
      DigOut(TEKLA, Souliss_T1n_Coil, SL_TEKLA);
      DigOut(HALO, Souliss_T1n_Coil, SL_HALO);
      DigOut(VENDEG, Souliss_T1n_Coil, SL_VENDEG);
      DigOut(FURDO, Souliss_T1n_Coil, SL_FURDO);
      
      // This pulls the PINs Low for blind control
      DigOut(TEKLAF, Souliss_T2n_Coil_Open, SB_TEKLA);
      DigOut(TEKLAL, Souliss_T2n_Coil_Close, SB_TEKLA);
      DigOut(HALOF, Souliss_T2n_Coil_Open, SB_HALO);
      DigOut(HALOL, Souliss_T2n_Coil_Close, SB_HALO);
    }

    FAST_1110ms() {
      Timer_Windows(SB_TEKLA);
      Timer_Windows(SB_HALO);
    }
    
    FAST_BridgeComms();

  }

}
