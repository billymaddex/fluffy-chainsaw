;
; DA1B.asm
;
; Created: 9/21/2019 3:51:24 PM
; Author : Billy
;


; Set some constants
.EQU	STARTADDS = 0X0200
.EQU	MOD5_YES_ADDS = 0X0300
.EQU	MOD5_NO_ADDS = 0X0500
.EQU	STARTNUM = 3
.EQU	COUNT = 250

start:
	LDI		XH, HIGH(STARTADDS)
	LDI		XL, LOW(STARTADDS)
	LDI		YH, HIGH(MOD5_YES_ADDS)
	LDI		YL, LOW(MOD5_YES_ADDS)
	LDI		ZH, HIGH(MOD5_NO_ADDS)
	LDI		ZL, LOW(MOD5_NO_ADDS)

	LDI		R20, 0
	LDI		R21, STARTNUM
	LDI		R22, COUNT

WORK_START:
	; Store current value to current RAM address and then increment RAM address
	ST		X+, R21
	; Copy current value to working register for modulus analysis
	MOV		R23, R21

MOD5TEST:
	; Subtract 5 from working register until zero or negative is reached
	SUBI	R23, 5
	BRLO	MOD5_NO
	BREQ	MOD5_YES

	; If not yet zero or negative repeat the subtraction
	RJMP	MOD5TEST

MOD5_YES:
	; Current value is divisible by 5. Sum it up and put it the the correct RAM address
	ADD		R16, R21
	ADC		R17, R20
	ST		Y+, R21
	RJMP	NEXT_VALUE

MOD5_NO:
	; Current value is not divisible by 5. Sum it up and put in the correct RAM address
	ADD		R18, R21
	ADC		R19, R20
	ST		Z+, R21
	RJMP	NEXT_VALUE

NEXT_VALUE:
	; Update current value, and counter. Repeat loop until count is zero
	DEC		R22
	BREQ	END_WORK
	INC		R21
	RJMP	WORK_START

END_WORK:
	RJMP	END_WORK