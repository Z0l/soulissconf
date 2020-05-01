#include "SoulissFramework.h"
#include "bconf/StandardArduino.h"          // Use a standard Arduino
#include "conf/ethW5100.h"                  // Ethernet through Wiznet W5100
#include "conf/usart.h"                     // USART RS485
#include "conf/Gateway.h"                   // The main node is the Gateway
//#include "conf/IPBroadcast.h"               // Needed for IP broadcasting

#define USARTDRIVER_INSKETCH
#define USARTBAUDRATE_INSKETCH
#define USART_BAUD9k6           1
#define USART_TXENABLE          1
#define USART_TXENPIN           2
#define USARTDRIVER             Serial
/*************/

// Include framework code and libraries
#include <SPI.h>

/*** All configuration includes should be above this line ***/
#include "Souliss.h"

// Define the network configuration according to your router settings
uint8_t ip_address[4]  = {192, 168, 1, 77};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};
#define myvNet_address  ip_address[3]       // The last byte of the IP address (77) is also the vNet address
#define Emelet 78
// Define the RS485 network configuration
#define myvNet_subnet   0xFF00
#define Gateway_RS485   0xCE01
#define Eloszoba        0xCE02
#define Nappali         0xCE03
#define Konyha          0xCE04
#define Halo            0xCE05
#define Terasz          0xCE06


// Local light slots
#define SL_ELOSZOBA 0
#define SL_NAPPALI 1
#define SL_ETKEZO 2
#define SL_HALO 3
#define SL_KONYHA 4
#define SL_FURDO 5
#define SL_KULSO1 6 //bejarat
#define SL_KULSO2 7 //terasz
#define SL_KULSO3 8  //kocsibeallo


// Local blind slots
#define SB_NAPPALI1 9
#define SB_NAPPALI2 10
#define SB_NAPPALI3 11
#define SB_HALO1 12
#define SB_HALO2 13
#define BL_TIMEOUT 0xBB

//cirkulacios szivattyu
#define SP_CIRKULACIOS 14
#define SP_GF_PUMP 15
#define SP_FF_PUMP 16
#define SP_HEATING 17

// Local light output PINs
#define ELOSZOBA 22
#define NAPPALI 23
#define ETKEZO 24
#define HALO 25
#define KONYHA 26
#define FURDO 27
#define KULSO1 28
#define KULSO2 29
#define KULSO3 30

// Local blind output PINs
#define NAPPALI1F 31
#define NAPPALI1L 32
#define NAPPALI2F 33
#define NAPPALI2L 34
#define NAPPALI3F 35
#define NAPPALI3L 36
#define HALO1F    37
#define HALO1L    38
#define HALO2F    39
#define HALO2L    40

#define CIRKULACIOS 50
#define GF_PUMP 51
#define FF_PUMP 52
#define HEATING 53

void setup()
{
  Initialize();

  // Set network parameters
  Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
  SetAsGateway(myvNet_address);                                   // Set this node as gateway for SoulissApp
  SetAddress(Gateway_RS485, myvNet_subnet, 0);                    // Set the address on the RS485 bus
  SetAddress(myvNet_address, myvNet_subnet, 0);                             // Define IP address
  //RoutingTable(Emelet, 0xCD00, 0);                              // Emeleti RS485 halozat

  // This node as gateway will get data from the Peer
  SetAsPeerNode(Emelet, 1);

  // Define PINs as output
  for (int thisPin = 22; thisPin < 54; thisPin++) {
      pinMode(thisPin, OUTPUT);  
  }

  // Drive PINs low by default
  for (int thisPin = 22; thisPin < 54; thisPin++) {
      digitalWrite(thisPin, LOW);  
  }
  


  // Define light logic
  Set_T11(SL_ELOSZOBA);
  Set_T11(SL_NAPPALI);
  Set_T11(SL_ETKEZO);
  Set_T11(SL_HALO);
  Set_T11(SL_KONYHA);  
  Set_T11(SL_FURDO);
  Set_T11(SL_KULSO1);
  Set_T11(SL_KULSO2);
  Set_T11(SL_KULSO3);

  // Define T22 logic for all motors
  Set_T22(SB_NAPPALI1);
  Set_T22(SB_NAPPALI2);
  Set_T22(SB_NAPPALI3);
  Set_T22(SB_HALO1);
  Set_T22(SB_HALO2);

  //define T11 for circulation pump
  Set_T11(SP_CIRKULACIOS);
  Set_T11(SP_GF_PUMP);
  Set_T11(SP_FF_PUMP);
  Set_T11(SP_HEATING);
}

void loop()
{
  
  EXECUTEFAST() {
    UPDATEFAST();

    FAST_110ms() {
      // This starts the lights logic
      Logic_T11(SL_ELOSZOBA);
      Logic_T11(SL_NAPPALI);
      Logic_T11(SL_ETKEZO);
      Logic_T11(SL_HALO);
      Logic_T11(SL_KONYHA);
      Logic_T11(SL_FURDO);
      Logic_T11(SL_KULSO1);
      Logic_T11(SL_KULSO2);
      Logic_T11(SL_KULSO3);
      
      // This starts the blind logic

      Souliss_Logic_T22(memory_map, SB_NAPPALI1, &data_changed, BL_TIMEOUT);
      Souliss_Logic_T22(memory_map, SB_NAPPALI2, &data_changed, BL_TIMEOUT);
      Souliss_Logic_T22(memory_map, SB_NAPPALI3, &data_changed, BL_TIMEOUT);
      Souliss_Logic_T22(memory_map, SB_HALO1, &data_changed, BL_TIMEOUT);
      Souliss_Logic_T22(memory_map, SB_HALO2, &data_changed, BL_TIMEOUT);
      

      // Pump logic
      Logic_T11(SP_CIRKULACIOS);
      Logic_T11(SP_GF_PUMP);
      Logic_T11(SP_FF_PUMP);
      Logic_T11(SP_HEATING);


      //This pulls the PINs High for light control
      DigOut(ELOSZOBA, Souliss_T1n_Coil, SL_ELOSZOBA);
      DigOut(NAPPALI, Souliss_T1n_Coil, SL_NAPPALI);
      DigOut(ETKEZO, Souliss_T1n_Coil, SL_ETKEZO);
      DigOut(HALO, Souliss_T1n_Coil, SL_HALO);
      DigOut(KONYHA, Souliss_T1n_Coil, SL_KONYHA);
      DigOut(FURDO, Souliss_T1n_Coil, SL_FURDO);
      DigOut(KULSO1, Souliss_T1n_Coil, SL_KULSO1);
      DigOut(KULSO2, Souliss_T1n_Coil, SL_KULSO2);
      DigOut(KULSO3, Souliss_T1n_Coil, SL_KULSO3);
      
      // This pulls the PINs Low for blind control
      DigOut(NAPPALI1F, Souliss_T2n_Coil_Open, SB_NAPPALI1);
      DigOut(NAPPALI1L, Souliss_T2n_Coil_Close, SB_NAPPALI1);
      DigOut(NAPPALI2F, Souliss_T2n_Coil_Open, SB_NAPPALI2);
      DigOut(NAPPALI2L, Souliss_T2n_Coil_Close, SB_NAPPALI2);
      DigOut(NAPPALI3F, Souliss_T2n_Coil_Open, SB_NAPPALI3);
      DigOut(NAPPALI3L, Souliss_T2n_Coil_Close, SB_NAPPALI3);
      DigOut(HALO1F, Souliss_T2n_Coil_Open, SB_HALO1);
      DigOut(HALO1L, Souliss_T2n_Coil_Close, SB_HALO1);
      DigOut(HALO2F, Souliss_T2n_Coil_Open, SB_HALO2);
      DigOut(HALO2L, Souliss_T2n_Coil_Close, SB_HALO2);

      //Pulling PIN high for circulation pump
      DigOut(CIRKULACIOS, Souliss_T1n_Coil, SP_CIRKULACIOS);
      DigOut(GF_PUMP, Souliss_T1n_Coil, SP_GF_PUMP);
      DigOut(FF_PUMP, Souliss_T1n_Coil, SP_FF_PUMP);
      DigOut(HEATING, Souliss_T1n_Coil, SP_HEATING);
    }

    FAST_510ms() {
        Timer_Windows(SB_NAPPALI1);
        Timer_Windows(SB_NAPPALI2);
        Timer_Windows(SB_NAPPALI3);
        Timer_Windows(SB_HALO1);
        Timer_Windows(SB_HALO2);
    }
    FAST_GatewayComms();

  }

EXECUTESLOW() {
    UPDATESLOW();
    SLOW_10s() {

    }
}
}
