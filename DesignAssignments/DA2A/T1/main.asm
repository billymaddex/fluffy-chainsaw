;
; DA2AT1.asm
;
; Created: 9/29/2019 3:15:15 PM
; Author : Billy
;


	;; Set GPIO ports
	;; Set DDR B,C,D to known initial value
	LDI		R25, 0x00
	OUT		DDRB, R25
	OUT		DDRC, R25
	OUT		DDRD, R25
	;; set PORT B,C,D to know initial value
	LDI		R25, 0xFF
	OUT		PORTB, R25
	OUT		PORTC, R25
	OUT		PORTD, R25
	;; set PORTB 2 and 3 to output
	SBI		DDRB, 3
	SBI		DDRB, 2
	;; set PINC 3 to input
	CBI		DDRC, 3

MAIN_LOOP:
	;; LED pulse
	;; turn the light on
	CBI		PORTB, 3
	;; call delay at 250ms
	LDI		R17, HIGH(250)
	LDI		R16, LOW(250)
	CALL		DELAY
	;; turn the light off
	SBI		PORTB, 3
	;; call delay at 375ms
	LDI		R17, HIGH(375)
	LDI		R16, LOW(375)
	CALL		DELAY

	;; if switch is not pressed, repeat the loop
	SBIC		PINC, 3
	RJMP		MAIN_LOOP

	;; otherwise, flash the other light
	;; turn the light on
	CBI		PORTB, 2
	;; call delay at 1333ms
	LDI		R17, HIGH(1333)
	LDI		R16, LOW(1333)
	CALL		DELAY
	;; turn the light off
	SBI		PORTB, 2	

	;; repeat the loop
	RJMP		MAIN_LOOP

	;; simple delay function
	;; takes input as 16-bit int in R17:R16
	;; delay is supposed to be in ms
DELAY:
	LDI		R20, 0
	MOV		R22, R17
	MOV		R21, R16
L3:
	LDI		R23, 0X2F
L2:
	LDI		R24, 0X70
	NOP
L1:
	DEC		R24
	BRNE		L1
	DEC		R23
	BRNE		L2
	SUBI		R21, 1
	SBCI		R22, 0
	BRNE		L3
	CP		R21, R20
	BRNE		L3

	RET
