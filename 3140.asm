	;; 3140 routines for concurrency
	.cdecls C,LIST,"msp430x22x4.h"
	.cdecls C,LIST,"3140_concur.h"

	.text
	.global process_blocked
	.global process_terminated
	.global process_select
	.global process_begin

process_terminated:
	bic.w #GIE,SR
	mov.w #0,R12
	jmp entry_for_dead_process
process_begin:
	bic.w #GIE,SR
	mov.w SP,_original_stack_pointer
	mov.w #0,R12
	jmp entry_for_dead_process
process_blocked:	
	push.w SR		; make the stack look like an interrupt occurred
	bic.w	#GIE,SR		; disable interrupts
	;; Timer interrupt
_process_timer_interrupt:
	;; Step 1: save registers
	push.w R4
	push.w R5
	push.w R6
	push.w R7
	push.w R8
	push.w R9
	push.w R10
	push.w R11
	push.w R12
	push.w R13
	push.w R14
	push.w R15
	;; call process_select(sp)
	mov.w SP,R12
entry_for_dead_process:		
	mov.w _original_stack_pointer,SP
	call #process_select
	cmp.w #0,R12		
	jnz lresume		; there is a next process:  continue
	;; no more processes to run
	mov.w _original_stack_pointer,SP
	ret
lresume:	
	mov.w R12,SP		; switch stacks
	pop.w R15
	pop.w R14
	pop.w R13
	pop.w R12
	pop.w R11
	pop.w R10
	pop.w R9
	pop.w R8
	pop.w R7
	pop.w R6
	pop.w R5
	pop.w R4
	reti			; return!
		
	.bss _original_stack_pointer,2,2

	.sect ".int09"
	.word _process_timer_interrupt
