;
; DA1A.asm
;
; Created: 9/16/2019 7:54:33 PM
; Author : Billy
;

;Set constants for the factors
.EQU	X1 = 0x1361
.EQU	X2 = 0xFDEC

start:
	;Load constant X1 into R25:R24
    LDI		R25, HIGH(X1)
	LDI		R24, LOW(X1)

	;Load constant X2 into R23:R22
	LDI		R23, HIGH(X2)
	LDI		R22, LOW(X2)

	;set R21 to 0 for oh so many reasons
	LDI		R21, 0

ADD_LOOP:
	;Accumulate R24:R25 into R20:R19:R18:R17
	ADD		R17, R24
	ADC		R18, R25
	ADC		R19, R21
	ADC		R20, R21

	;Decrement R23:R22 each time X1 is accumulated
	SUBI	R22, 1
	SBC		R23, R21

	;Repeat the accumulation, unless both R23 and R22 are zero
	CP		R22, R21
	BRNE	ADD_LOOP
	CP		R23, R21
	BRNE	ADD_LOOP

DONE:
	;Wait here when done
	RJMP	DONE