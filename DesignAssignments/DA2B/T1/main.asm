;
; DA2BT1.asm
;
; Created: 10/05/2019 1:15:15 PM
; Author : Billy
;

.INCLUDE <M328PBDEF.INC>

.ORG 0
	JMP		MAIN
.ORG 0X02
	JMP		EX0_ISR
.ORG 0x08
	JMP		PCINT11_ISR

MAIN:
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

	;; configure interupt handling
	;; enable INT0
	LDI		R20, 1 << INT0
	OUT		EIMSK, R20
	;; set to falling edge trigger
	LDI		R20, 0X02
	STS		EICRA, R20
	;; enable PCINT11 (PINC.3)
	LDI		R20, 0x08
	STS		PCMSK1, R20
	LDI		R20, 1 << PCIE1
	STS		PCICR, R20
	;; enable global interupt
	SEI

MAIN_LOOP:
	;; LED pulse
	;; turn the light on
	CBI		PORTB, 3
	;; call delay at 250ms
	LDI		R17, HIGH(250)
	LDI		R16, LOW(250)
	CALL	DELAY
	;; turn the light off
	SBI		PORTB, 3
	;; call delay at 375ms
	LDI		R17, HIGH(375)
	LDI		R16, LOW(375)
	CALL	DELAY

	;; repeat the loop
	RJMP	MAIN_LOOP

	;; pin change interupt handler function
PCINT11_ISR:
	;; if switch is not pressed, exit the function
	SBIC	PINC, 3
	RETI

	;; switch is pressed, flash light 2
	CALL	LIGHT2
	RETI

	;; external interupt handler function
EX0_ISR:
	;; if switch is not pressed, exit the function
	SBIC	PIND, 2
	RETI

	;; switch is pressed, flash light 2
	CALL	LIGHT2
	RETI

	;; long flash on secondary LED
LIGHT2:
	;; save all delay function registers
	PUSH	R16
	PUSH	R17
	PUSH	R20
	PUSH	R21
	PUSH	R22
	PUSH	R23
	PUSH	R24

	;; turn the light on
	CBI		PORTB, 2
	;; call delay at 1333ms
	LDI		R17, HIGH(1333)
	LDI		R16, LOW(1333)
	CALL	DELAY
	;; turn the light off
	SBI		PORTB, 2	

	;; restore all delay function registers
	POP		R24
	POP		R23
	POP		R22
	POP		R21
	POP		R20
	POP		R17
	POP		R16

	RET

	;; simple delay function
	;; takes input as 16-bit int in R17:R16
	;; delay is supposed to be in ms
DELAY:
	;;RET	;; FOR DEBUG ONLY
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
	BRNE	L1
	DEC		R23
	BRNE	L2
	SUBI	R21, 1
	SBCI	R22, 0
	BRNE	L3
	CP		R21, R20
	BRNE	L3

	RET
