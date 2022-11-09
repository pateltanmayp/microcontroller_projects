# Microcontroller Projects

## 6502 Computer Build
I built a 6502 computer from scratch and wrote a program for it. The program allows a user to roll an 8 sided-dice by interacting with some peripherals. The program was loaded into the EEPROM manually using an EEPROM programmer I built. Each Assembly command was converted to a hexadecimal equivalent and loaded into individual locations in the program storage as binary. The [6502 Assembly language](http://www.6502.org/tutorials/65c02opcodes.html) was used.

## Sequencer
Produces a sequencer program that displays a dynamic LED pattern. Users can interact with and change the LED pattern through push buttons. All code is implemented in the [PIC Assembly lanuage](http://ww1.microchip.com/downloads/en/DeviceDoc/33014K.pdf).

## Message Wand
Uses C Interrupts to display a message of choice when the wand is waved back and forth. As the wand is waved in one direction, the array of LEDs rapidly changes its illumination pattern every frame to trick the brain into thinking it displayed a full 2-dimensional message (a word). An inertial sensor detects changes in direction and plays the frames in reverse order when the wand is waved backward. Therefore, when waved rapidly back and forth, the device will appear to be showing a customized message to the user.

## Reaction Tester
Allows users to test their reaction times in a physical setup. Conceptually, this game is similar to Whack-a-Mole, but with LEDs!