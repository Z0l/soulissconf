// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"
// Configure the framework
#include "bconf/StandardArduino.h"          // Use a standard Arduino
#include "conf/usart.h"                     // USART RS485

/*************/
// Use the following if you are using an RS485 transceiver with 
// transmission enable pin, otherwise delete this section.
//
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

// Define the RS485 network configuration
#define myvNet_subnet   0xFF00
#define Gateway_RS485   0xDE01
#define GLent           0xDE02
#define GFent           0xDE03
#define Tekla           0xDE04
#define Halo            0xDE05
#define Vendeg          0xDE06
#define Furdo           0xDE07


#define LSLOT_LAMP1   0   //local light slot (doesn't matter)
#define RSLOT_LAMP1   2   //remote light slot (this is being changed on the remote)
#define PIN_LAMP1  5      //which PIN the switch is connected to


void setup()
{   
    Initialize();
    // Set network parameters
    SetAddress(Halo, myvNet_subnet, Emelet_RS485);      

    // Set the typical logic to handle the local button
    Set_T11(LSLOT_LAMP1);

    // Define inputs
    pinMode(PIN_LAMP1, INPUT_PULLUP);                  // Hardware pulldown required
    pinMode(12, OUTPUT);  // additional GND
    digitalWrite(12, LOW);

}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   

        FAST_50ms() {
          
            if(DigIn(PIN_LAMP1, Souliss_T1n_ToggleCmd, LSLOT_LAMP1))
              {
                Send(Emelet_RS485, RSLOT_LAMP1, Souliss_T1n_ToggleCmd);
                ResetInput(LSLOT_LAMP1);
              }

            Logic_T11(LSLOT_LAMP1); //start the local button logic
                  
            
        }

        FAST_PeerComms();
        
    }
    
    
}
