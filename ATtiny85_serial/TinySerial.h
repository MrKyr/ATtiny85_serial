// Serial Transmission Example for the ATtiny85
// by Ido Gendel, 2015
// Share and enjoy!
// https://www.idogendel.com/en/archives/406
// https://pastebin.com/cKn6G2mL
// Works only in 16Mhz clock internal on ATtiny85

// Make sure system clock is 16MHz

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

//#define F_CPU 16000000UL // Defined in Makefile

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>        // Fix <implicit declaration of function 'itoa'> error

volatile struct {
    uint8_t dataByte, bitsLeft,
    pin, done;
} txData = {0, 0, 0, 0};

//---------------------------------------------------------
void sendBySerial(const uint8_t data) {
    
    txData.dataByte = data;
    txData.bitsLeft = 10;
    txData.done = 0;
    // Reset counter
    TCNT0 = 0;
    // Activate timer0 A match interrupt
    TIMSK = 1 << OCIE0A;
    
} // sendBySerial

//---------------------------------------------------------
// Blocking
void sendStrBySerial(char *p) {
    
    while (*p != '\0') {
        sendBySerial(*p++);
        while (!txData.done);
    } // while
    
} // sendStrBySerial

// Print func by kyr
void Print(char p[]) {
    sendStrBySerial(p);
} // Print

// Println func by kyr (with new line at the end)
void Println(char p[]) {
    sendStrBySerial(p);
    sendBySerial(10);   // new line
    _delay_ms(2);       // the minimum delay for repeating Print
} // Println

// Convert numbers to chars to print to serial by kyr
void PrintNum(long num) {
    char str[10];
    ltoa(num, str, 10);
    sendStrBySerial(str);
} // PrintNum

//---------------------------------------------------------
void initSerial(const uint8_t portPin) {
    
    txData.pin = portPin;
    // Define pin as output
    DDRB = 1 << txData.pin;
    // Set it to the default HIGH
    PORTB |= 1 << txData.pin;
    // Set top value for counter 0
    OCR0A = 26;
    // No A/B match output; just CTC mode
    TCCR0A = 1 << WGM01;
    // Set prescaler to clk/64
    TCCR0B = (1 << CS01) | 1 << (CS00);
    
    sei(); // Enable interupts
    
} // initSerial;

//---------------------------------------------------------
// Timer 0 A-match interrupt
ISR(TIMER0_COMPA_vect) {
    
    uint8_t bitVal;
    
    switch (txData.bitsLeft) {
            
        case 10: bitVal = 0; break;
        case  1: bitVal = 1; break;
        case  0: TIMSK &= ~(1 << OCIE0A);
            txData.done = 1;
            return;
            
        default:
            bitVal = txData.dataByte & 1;
            txData.dataByte >>= 1;
            
    } // switch
    
    if (bitVal) PORTB |= (1 << txData.pin);
    else PORTB &= ~(1 << txData.pin);
    --txData.bitsLeft;
    
}

