/*
 * Description: Reaction Tester Whack-a-Mole Game!
 *		Press a start button to start the game. A sequence of LEDs will light up after 
 *		a random delay time. Press the stop button and observe the number of lit LEDs.
 *		The program caters itself to the user's reaction time, slowing down or speeding
 *		up when needed.
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

// Import necessary libraries (math.h is needed to generate the random number)
#include <xc.h>
#include <math.h>
#define _XTAL_FREQ 1000000  // Define the crystal frequency

// Integer array stores the state of each LED (0 is off, 1 is on)
int LEDArray[] = {0, 0, 0, 0, 0, 0, 0, 0};
int numLEDs = 8; // Number of LEDs used in the program

// Initialize all necessary special function registers 
void initialize() {
    // Clear the analog select registers
    ANSEL = 0;
    ANSELH = 0;
    
    // Enable weak pull-ups for all pins in the PORTB register
    OPTION_REGbits.nRABPU = 0;
    WPUB = 0b11111111;
    
    // Set PORTC to output and PORTB to input
    TRISC = 0b00000000;
    TRISB = 0b11111111;
    
    // Initially set both PORTs to off (i.e. clear any "noise")
    PORTC = 0b00000000;
    PORTB = 0b00000000;
}

// Create a specified time delay (in milliseconds)
void delay(int msDelay) {
    
    // Loop the specified number of times
    for (int i = 0; i < msDelay; i++) {
        __delay_ms(1);  // Each iteration of the loop delays the program by 1 millisecond
    }
}

// Set the status of a specific LED to the specified value (0 is off, 1 is on)
void setLED(int LEDNum, int value) {
    LEDArray[LEDNum] = value;
}

// Convert the LED status values to actual values in PORTC
void lightLEDs() {
    PORTCbits.RC0 = LEDArray[0];
    PORTCbits.RC1 = LEDArray[1];
    PORTCbits.RC2 = LEDArray[2];
    PORTCbits.RC3 = LEDArray[3];
    PORTCbits.RC4 = LEDArray[4];
    PORTCbits.RC5 = LEDArray[5];
    PORTCbits.RC6 = LEDArray[6];
    PORTCbits.RC7 = LEDArray[7];
}

// Run the primary logic of the program
void main(void) {
    initialize(); 	    // Initialize all necessary SFRs
    
    int previousScore;      // Represents the number of LEDs lit up in the previous round (governs delay time)
    int delayTime = 0;      // Time delay between successive LEDs (dependent on previous score)
    int randomNumSeed = 0;  // Seed used to generate the random number
    int randomNum = 0;      // The random number used for the initial time delay
    
    while(1) {
        randomNumSeed++; // The seed is incremented in the background when no other function is being performed
        
        // Start the LED sequence if the start button (connected to RB5) is pressed
        if (PORTBbits.RB5 == 0) {
            randomNum = (int) (5000 * sin(randomNumSeed) + 5000); // Convert the seed to a random number
            delay(randomNum);                // Create a random time delay (between 0 and 10 seconds)
            delayTime = previousScore * 60;  // Modify the delay time between successive LEDs as per previous performances
            
            // Light successive LEDs
            for (int i = 0; i < numLEDs; i++) {
                delay(delayTime);  // Create a short time delay between LEDs
                setLED(i, 1);      // Set the status of the current LED to on
                lightLEDs();       // Display the updated LED pattern
                
                // Stop the LED sequence if the stop button (connected to RB4) is pressed
                if (PORTBbits.RB4 == 0) {
                    previousScore = i; // Update the previous score to the current score
                    break;
                }
            }
            delay(5000); // Keep the current score frozen for 5 seconds
            
            // Reset all LEDs back to off
            for (int i = 0; i < numLEDs; i++) {
                setLED(i, 0);
            }
            lightLEDs();
        }
    }
    
    return;
}