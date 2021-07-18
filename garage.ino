#include "SoulissFramework.h"
#include "bconf/StandardArduino.h"
#include "conf/ethENC28J60.h"


#include <SPI.h>

/*** All configuration includes should be above this line ***/
#include "Souliss.h"

//define IP network config
uint8_t ip_address[4]  = {192, 168, 1, 79};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};
#define myvNet_address  ip_address[3]
#define gw_address 77
#define myvNet_subnet   0xFF00

// Local light slot
#define SL_GARAGELIGHT 0

// Local Light output PIN
#define GARAGE 7

// Local button input PIN
#define B_GARAGE 5

void setup() {
  
  Initialize();
  
  Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
  SetAddress(myvNet_address, myvNet_subnet, gw_address);

  pinMode(GARAGE, OUTPUT);
  pinMode(B_GARAGE, INPUT);

    // Drive lights low by default
  //digitalWrite(GARAGE, LOW);

  // Define light logic
  Set_T11(SL_GARAGELIGHT);
}

void loop() {
    EXECUTEFAST() {
    UPDATEFAST();

    FAST_50ms() {

      if(DigIn(B_GARAGE, Souliss_T1n_ToggleCmd, SL_GARAGELIGHT))
        DigOut(GARAGE, Souliss_T1n_Coil, SL_GARAGELIGHT);

      Logic_T11(SL_GARAGELIGHT);

    }

    FAST_PeerComms();
    
    }

    EXECUTESLOW() { 
      
        UPDATESLOW();

        SLOW_10s() {  // Process the timer every 10 seconds  
            Timer_SimpleLight(SL_GARAGELIGHT);
        }     
    }
}
