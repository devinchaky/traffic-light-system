;Software delay functions
; Devin Chaky
; 2/10/23
   AREA    |.text|, CODE, READONLY, ALIGN=2
   THUMB
   EXPORT Delay
;*********Delay**********************
; Software delay loop
; Input: R0 is count, the amount to delay in 100ns
; Output: none
Delay NOP ;dummy operation
      NOP
      NOP
      NOP
      SUBS R0,R0,#1
      BNE  Delay
      BX   LR
      
   EXPORT Wait10ms
;*********Wait10ms**********************
; Software delay loop
; Input: R0 is count, the amount to delay in 10ms
; Output: none
Wait10ms
; implement this using AAPCS
	  PUSH {R1, R2, LR}
	  
	  MOV R1, #10000
	  MOV R2, #10
	  MUL R1, R1, R2
	  MUL R0, R1
	  BL Delay

	  POP {R1, R2, LR}
      BX  LR
      END      
