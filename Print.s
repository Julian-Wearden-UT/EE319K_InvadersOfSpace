; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 
i EQU 0
    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
	PRESERVE8
    THUMB

  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
		PUSH {R4,R5}
		
		SUB SP, #4
		MOV R5, SP
		
		MOV R4, LR
		MOV R2, #10
		MOV R3, #0	;counter
		STR R3, [R5, #i]
		
		CMP R0, #0x00
		BEQ zero
		
load	CMP R0, #0
		BEQ print
		
		MOV R1, R0
		BL Modulo
		ADD R1, #0x30
		PUSH{R1}
		UDIV R0, R2
		ADD R3, #1
		STR R3, [R5, #i]
		
		B load
		
print	CMP R3, #0
		BEQ finish
		

		POP{R0}
		PUSH {R0-R4, LR}
		BL ST7735_OutChar
		POP{R0-R4, LR}
		SUBS R3, #1
		STR R3, [R5, #i]
		B print
		
finish	ADD SP, #4
		MOV LR, R4
		POP{R4,R5}
		BX  LR
		
		
zero	ADD R0, #0x30
		PUSH {R0-R4, LR}
		BL ST7735_OutChar
		POP{R0-R4, LR}
		B finish

      BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix

     PUSH {R4}
		
		SUB SP, #4
		
		MOV R4, LR
		MOV R3, #9999
		STR R3, [SP, #i]
		
		CMP R0, R3
		BHI over
		

		MOV R1, R0
		MOV R3, #1000
		STR R3, [SP, #i]
		
		UDIV R2, R1, R3
		ADD R0, R2, #0x30
		PUSH {R0-R4, LR}
		BL ST7735_OutChar
		POP{R0-R4, LR}
		
		MOV R2, #1000
		BL Modulo
		
		MOV R0, #0x2E
		PUSH {R0-R4, LR}
		BL ST7735_OutChar
		POP{R0-R4, LR}
		
		MOV R3, #100
		STR R3, [SP, #i]
		
		UDIV R2, R1, R3
		ADD R0, R2, #0x30
		PUSH {R0-R4, LR}
		BL ST7735_OutChar
		POP{R0-R4, LR}
		
		MOV R2, #100
		BL Modulo
		
		MOV R3, #10
		STR R3, [SP, #i]
		
		UDIV R2, R1, R3
		ADD R0, R2, #0x30
		PUSH {R0-R4, LR}
		BL ST7735_OutChar
		POP{R0-R4, LR}
		
		MOV R2, #10
		BL Modulo
		
		ADD R0, R1, #0x30
		PUSH {R0-R4, LR}
		BL ST7735_OutChar
		POP{R0-R4, LR}
		B done
		
over	MOV R0, #0x2A
		PUSH {R0-R4, LR}
		BL ST7735_OutChar
		POP{R0-R4, LR}
		MOV R0, #0x2E
		PUSH {R0-R4, LR}
		BL ST7735_OutChar
		POP{R0-R4, LR}
		MOV R0, #0x2A
		PUSH {R0-R4, LR}
		BL ST7735_OutChar
		POP{R0-R4, LR}
		PUSH {R0-R4, LR}
		BL ST7735_OutChar
		POP{R0-R4, LR}
		PUSH {R0-R4, LR}
		BL ST7735_OutChar
		POP{R0-R4, LR}
		
done	ADD SP, #4
		MOV LR, R4
		POP {R4}
		BX   LR
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

Modulo
	;Input
		;R1 contains dividend, then remainder
		;R2 contains divisor
	;Output
		;R1 contains remainder
	
div		CMP R1, R2
		BCC exit
		SUB R1, R2
		B div
exit	BX LR

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
