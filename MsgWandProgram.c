/*
 * Project: Message Wand
 * Description: Displays a message when the wand is waved back and forth (using interrupts).
 */

// CONFIG
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)


#include <xc.h>                 // Import the necessary library
#define _XTAL_FREQ 4000000      // Set the clock frequency (4MHz)

char totalFrames = 30;          // Total frames that need to be displayed
char currentFrame = 0;          // Represents which frame is being displayed currently
char msDelayTime = 4;           // Time between successive frames
bit dir = 0;                    // 1 is the forward direction, 0 is the reverse direction

// Array stores the values (frame-by-frame) that need to be outputted (each row is a letter)
char PORTCValues[] = {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0,    // Space
                      0b11111111, 0b00001000, 0b00001000, 0b00001000, 0b11111111, 0,    // H
                      0b01111110, 0b10000001, 0b10000001, 0b10000001, 0b01111110, 0,    // O
                      0b11111111, 0b00000001, 0b00000111, 0b00000001, 0b11111111, 0,    // W
                      0b11111111, 0b10000001, 0b10000001, 0b10000001, 0b01111110, 0,    // D
                      0b11100000, 0b00010000, 0b00001111, 0b00010000, 0b11100000, 0,    // Y
                      };

// Initialize all special function registers
void initialize(void) {
    ANSEL = 0;  		// Disable analog ports
    ANSELH = 0;
    OPTION_REGbits.nRABPU = 0;  // Enable weak pull-ups
    WPUB = 0b11111111;          // Enable pull-ups specifically in PORTB
    TRISC = 0b00000000;         // Configure TRISC to output
    TRISB = 0b11111111;         // Configure TRISB to input
    PORTC = 0b00000000;         // Clear both ports temporarily (clear any noise)
    PORTB = 0b00000000;
    
    INTCONbits.RABIE = 1;       // Enable change interrupts within PORTA/B
    INTCONbits.GIE = 1;		// Enable all unmasked interrupts
    INTCONbits.RABIF = 0;       // Clear the interrupt flag
    IOCBbits.IOCB5 = 1;         // Instruct the PIC to detect interrupts on change from PORTB bit 5
    return;
}

// Display the current frame
void displayFrame(int frameNum) {
    PORTC = PORTCValues[frameNum];  // Set PORTC to the current frame's LED pattern
    return;
}

// Create a delay of the specified amount (in ms)
void delay(int msDelay) {
    
    // Delay by 1 ms for each iteration
    for (int i = 0; i < msDelay; i++) {
        __delay_ms(1);
    }
    return;
}

// Interrupt service routine (called when there is a change on the RB5 line)
void __interrupt() isr(void) {
    dir = 1 - PORTBbits.RB5;    // Invert the direction bit
    INTCONbits.RABIF = 0;       // Clear the interrupt flag
    return;
}

// Main logic of the program
void main(void) {
    
    // Main loop
    while (1) {
        
        // If forward direction
        if (dir == 1) {
            displayFrame(currentFrame);   // Display the current frame  
            delay(msDelayTime);           // Delay 4 ms
            
            // Reset the current frame if it reaches the max
            if (currentFrame == totalFrames - 1) {
                currentFrame = -1;
            }
            currentFrame++;               // Increment the current frame          
        }
        
        // If reverse direction
        else if (dir == 0) {
            currentFrame = 0;       	  // Reset the current frame back to 0
            PORTC = 0b00000000;     	  // Keep all LEDs off
        }
    }
    return;
}
