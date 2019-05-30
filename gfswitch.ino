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
#define Gateway_RS485   0xCE01
#define Eloszoba        0xCE02
#define Nappali         0xCE03
#define Konyha          0xCE04
#define Halo            0xCE05
#define Terasz          0xCE06


#define LSLOT_LAMP1   0   //local light slot (doesn't matter)
#define LSLOT_LAMP2   1   //local light slot #2 (doesn't matter)
#define RSLOT_LAMP1   7   //remote light slot (this is being changed on the remote)
#define RSLOT_LAMP2   5   //remote light slot (this is being changed on the remote)
#define PIN_LAMP1  5      //which PIN the switch is connected to
#define PIN_LAMP2  6      //which PIN the other switch is connected to


void setup()
{   
    Initialize();
    
    // Set network parameters
    SetAddress(Terasz, myvNet_subnet, Gateway_RS485);      

    // Set the typical logic to handle the local button
    Set_T11(LSLOT_LAMP1);
//    Set_T11(LSLOT_LAMP2);

    // Define inputs
    pinMode(PIN_LAMP1, INPUT_PULLUP);                  // Hardware pulldown required
//    pinMode(PIN_LAMP2, INPUT_PULLUP);                  // Hardware pulldown required
    pinMode(11, OUTPUT);                               // additional GND
    pinMode(12, OUTPUT);                               // additional GND  
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);

}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   

        FAST_50ms() {
        
            if(DigIn(PIN_LAMP1, Souliss_T1n_ToggleCmd, LSLOT_LAMP1)) //if the local button is pressed
            {
                Send(Gateway_RS485, RSLOT_LAMP1, Souliss_T1n_ToggleCmd); //send this command to the RS485 gateway
                ResetInput(LSLOT_LAMP1);
            }
            /*if(DigIn(PIN_LAMP2, Souliss_T1n_ToggleCmd, LSLOT_LAMP2)) //if the local button #2 is pressed
              {
                Send(Gateway_RS485, RSLOT_LAMP2, Souliss_T1n_ToggleCmd); //send this command to the RS485 gateway
                ResetInput(LSLOT_LAMP2);
              }*/
            
            Logic_T11(LSLOT_LAMP1); //start the local button logic
            //Logic_T11(LSLOT_LAMP2); //start the local button logic #2
            
        }

        FAST_PeerComms();
        
    }
    
    
}
