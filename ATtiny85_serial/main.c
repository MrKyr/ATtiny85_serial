// Serial Transmission Example for the ATtiny85
// by Ido Gendel, 2015
// Share and enjoy!

/*
 *                     ATtiny85 16Mhz PLL
 *                        ╔═══╦═══╗
 *    (RESET)(ADC0)(PB5) -║1     8║- (VCC)(+)
 *    (XTAL1)(ADC3)(PB3) -║2     7║- (PB2)(A1)(SCK)(SCL)(ADC1)
 *    (XTAL2)(ADC2)(PB4) -║3     6║- (PB1)(MISO)(OC0B)(OC1A) -------> [FTDI](RX) or [Arduino](pin1)
 *              (GND)(-) -║4     5║- (PB0)(MOSI)(SDA)(OC0A)
 *                        ╚═══════╝
 *
 *         $ cat /dev/cu.usbserial-A5XK3RJT (FTDI's device name)
 */

// Make sure system clock is 16MHz
// #define in Makefile

#include "TinySerial.h"
#include <avr/eeprom.h>
#include <util/delay.h>

int main(void) {
    
    OSCCAL = eeprom_read_byte(0); // Fix clock; 74
    initSerial(PB1);              // Initialize serial
    
    // Loop
    while(1) {
        
        for (int i = 0; i <= 99; i++ ) {
            _delay_ms(50); // minimum delay is 2
            Print("No. "); // String
            PrintNum(i);   // Number
            Println("");   // new line
        }
        
    } // while
    
} // main
