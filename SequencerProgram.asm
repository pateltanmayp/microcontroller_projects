    ;***************************************************************************************;
    ;	Name: Tanmay Patel							    									;
    ;	Description: An LED Sequencer that responds to user inputs (via buttons). Buttons   ;
    ;		     can change the speed of the sequence.				    					;
    ;***************************************************************************************;
    
    #include "p16f690.inc"		; Import the necessary library
    
    ; Set the configuration bits
    __config (_INTRC_OSC_NOCLKOUT & _WDT_OFF & _PWRTE_OFF & _MCLRE_OFF & _CP_OFF& _BOR_OFF & _IESO_OFF & _FCMEN_OFF)
    
    cblock 0x20				; Declare key "variables" and assign them memory (RAM) locations
	workingStore			; Temporarily store the working register during interrupts
	statusStore		        ; Temporarily store the status register during interrupts
	innerDelay			; Number of iterations for the inner delay loop
	outerDelay		        ; Number of iterations for the outer delay loop
	temp				; Used to transfer a new value to the innerDelay variable when a button is pressed
    endc
    
    org 0				; Start writing the program at location 0 (in the program memory)
start
	goto initalize			; Jump to the initialize subroutine

interrupt
    org 4				; Start writing the interrupt instructions at location 4 (in the program memory)
	goto isr			; In the event of an interrupt, jump to the interrupt service routine
	
initialize				; Initialize the necessary SFR/RAM locations
	bsf	    STATUS,RP0		; Access register page 1
	bcf	    STATUS,RP1
	clrf	    TRISC		; Configure PORTC to output
	bsf	    TRISB,RB5		; Configure PORTB (bits 5 and 6) to input
	bsf	    TRISB,RB6
	bcf	    OPTION_REG,7	; Set the nRABPU bit (pull-up enable) in the option register
	movlw	    0x71
	movwf	    OSCCON		; Set the clock frequency to 8 MHz
	
	bcf	    STATUS,RP0		; Access register page 2
	bsf	    STATUS,RP1
	clrf	    ANSEL		; Disable all analog ports
	clrf	    ANSELH
	movlw	    0xFF		; Enable the weak pull-ups in PORTB
	movwf	    WPUB
	bsf	    IOCB,5		; Enable interrupts-on-change for bits 5 and 6 of PORTB
	bsf	    IOCB,6
	
	bcf	    STATUS,RP1		; Access register page 0
	clrf	    PORTC		; Clear all bits in PORTC and PORTB (clear any "noise")
	clrf	    PORTB
	bsf	    INTCON,GIE		; Enable all unmasked interrupts
	bsf	    INTCON,RABIE	; Enable change interrupts in PORTA/B
	bcf	    INTCON,RABIF	; Clear the interrupt flag
	clrf	    outerDelay		; Initially clear all "variable" locations
	clrf	    innerDelay
	clrf	    temp
	
main					; Create an LED sequence
	bcf	    PORTC,RC3		; Clear any previous LEDs
	bcf	    PORTC,RC4
	bsf	    PORTC,RC0		; Turn on the two outermost LEDs
	bsf	    PORTC,RC7
	call	    delay		; Create a short time delay
	bcf	    PORTC,RC0		; Repeat with the next set of LEDs
	bcf	    PORTC,RC7
	bsf	    PORTC,RC1
	bsf	    PORTC,RC6
	call	    delay
	bcf	    PORTC,RC1
	bcf	    PORTC,RC6
	bsf	    PORTC,RC2
	bsf	    PORTC,RC5
	call	    delay
	bcf	    PORTC,RC2
	bcf	    PORTC,RC5
	bsf	    PORTC,RC3
	bsf	    PORTC,RC4
	call	    delay
	goto	    main
	
delay					; Outer delay loop
	movf	    temp,w		; Move a potentially modified delay value to the inner loop counter
	movwf	    innerDelay
	call	    delay_inner		; Jump to the inner delay loop
	decfsz	    outerDelay,f	; Decrement the outer loop counter
	goto	    delay		; Repeat the outer loop for as long as the loop counter is not 0
	return
	
delay_inner				; Inner delay loop
	decfsz	    innerDelay,f	; Decrement the inner loop counter
	goto	    delay_inner		; Repeat the inner loop for as long as the loop counter is not 0
	return
	
isr					; Interrupt service routine
	movwf	    workingStore	; Transfer the contents of the working register to a temporary RAM location
	movf	    STATUS,w		; Transfer the contents of the status register to a temporary RAM location
	movwf	    statusStore
	
	btfss	    PORTB,RB5		; Decrease the delay time if the button connected to RB5 is pressed
	call	    decrease_delay
	
	btfss	    PORTB,RB6		; Reset the delay time if the button connected to RB6 is pressed
	call	    reset_delay
	
	movf	    statusStore,w	; Restore the contents of the status register
	movwf	    STATUS
	swapf	    workingStore,f	; Restore the contents of the working register (using swapf)
	swapf	    workingStore,w
	bcf	    INTCON,RABIF	; Clear the interrupt flag (so new interrupts can be detected)
	retfie
	
decrease_delay				; Decrease the delay time
	movlw	    0x3F		; Change the value of the inner loop counter and store it in the temp variable
	movwf	    temp
	return

reset_delay				; Reset the delay time
	clrf	    temp		; Reset the value of the inner loop counter (to 0, for maximum iterations)
	return
	
end_prog				; End the program
	end				; This statement is a necessary formality
