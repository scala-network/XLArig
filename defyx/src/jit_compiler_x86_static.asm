; Copyright (c) 2018-2019, tevador <tevador@gmail.com>
;
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
; 	* Redistributions of source code must retain the above copyright
; 	  notice, this list of conditions and the following disclaimer.
; 	* Redistributions in binary form must reproduce the above copyright
; 	  notice, this list of conditions and the following disclaimer in the
; 	  documentation and/or other materials provided with the distribution.
; 	* Neither the name of the copyright holder nor the
; 	  names of its contributors may be used to endorse or promote products
; 	  derived from this software without specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
; FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
; DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
; SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
; CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
; OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
; OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

IFDEF RAX

_RANDOMX_JITX86_STATIC SEGMENT PAGE READ EXECUTE

PUBLIC defyx_program_prologue
PUBLIC defyx_program_loop_begin
PUBLIC defyx_program_loop_load
PUBLIC defyx_program_start
PUBLIC defyx_program_read_dataset
PUBLIC defyx_program_read_dataset_sshash_init
PUBLIC defyx_program_read_dataset_sshash_fin
PUBLIC defyx_dataset_init
PUBLIC defyx_program_loop_store
PUBLIC defyx_program_loop_end
PUBLIC defyx_program_epilogue
PUBLIC defyx_sshash_load
PUBLIC defyx_sshash_prefetch
PUBLIC defyx_sshash_end
PUBLIC defyx_sshash_init
PUBLIC defyx_program_end
PUBLIC defyx_reciprocal_fast

include asm/configuration.asm

RANDOMX_SCRATCHPAD_MASK     EQU (RANDOMX_SCRATCHPAD_L3-64)
RANDOMX_DATASET_BASE_MASK   EQU (RANDOMX_DATASET_BASE_SIZE-64)
RANDOMX_CACHE_MASK          EQU (RANDOMX_ARGON_MEMORY*16-1)
RANDOMX_ALIGN               EQU 4096
SUPERSCALAR_OFFSET          EQU ((((RANDOMX_ALIGN + 32 * RANDOMX_PROGRAM_SIZE) - 1) / (RANDOMX_ALIGN) + 1) * (RANDOMX_ALIGN))

ALIGN 64
defyx_program_prologue PROC
	include asm/program_prologue_win64.inc
	movapd xmm13, xmmword ptr [mantissaMask]
	movapd xmm14, xmmword ptr [exp240]
	movapd xmm15, xmmword ptr [scaleMask]
	jmp defyx_program_loop_begin
defyx_program_prologue ENDP

ALIGN 64
	include asm/program_xmm_constants.inc

ALIGN 64
defyx_program_loop_begin PROC
	nop
defyx_program_loop_begin ENDP

defyx_program_loop_load PROC
	include asm/program_loop_load.inc
defyx_program_loop_load ENDP

defyx_program_start PROC
	nop
defyx_program_start ENDP

defyx_program_read_dataset PROC
	include asm/program_read_dataset.inc
defyx_program_read_dataset ENDP

defyx_program_read_dataset_sshash_init PROC
	include asm/program_read_dataset_sshash_init.inc
defyx_program_read_dataset_sshash_init ENDP

defyx_program_read_dataset_sshash_fin PROC
	include asm/program_read_dataset_sshash_fin.inc
defyx_program_read_dataset_sshash_fin ENDP

defyx_program_loop_store PROC
	include asm/program_loop_store.inc
defyx_program_loop_store ENDP

defyx_program_loop_end PROC
	nop
defyx_program_loop_end ENDP

ALIGN 64
defyx_dataset_init PROC
	push rbx
	push rbp
	push rdi
	push rsi
	push r12
	push r13
	push r14
	push r15
	mov rdi, qword ptr [rcx] ;# cache->memory
	mov rsi, rdx ;# dataset
	mov rbp, r8  ;# block index
	push r9      ;# max. block index
init_block_loop:
	prefetchw byte ptr [rsi]
	mov rbx, rbp
	db 232 ;# 0xE8 = call
	dd SUPERSCALAR_OFFSET - distance
	distance equ $ - offset defyx_dataset_init
	mov qword ptr [rsi+0], r8
	mov qword ptr [rsi+8], r9
	mov qword ptr [rsi+16], r10
	mov qword ptr [rsi+24], r11
	mov qword ptr [rsi+32], r12
	mov qword ptr [rsi+40], r13
	mov qword ptr [rsi+48], r14
	mov qword ptr [rsi+56], r15
	add rbp, 1
	add rsi, 64
	cmp rbp, qword ptr [rsp]
	jb init_block_loop
	pop r9
	pop r15
	pop r14
	pop r13
	pop r12
	pop rsi
	pop rdi
	pop rbp
	pop rbx
	ret
defyx_dataset_init ENDP

ALIGN 64
defyx_program_epilogue PROC
	include asm/program_epilogue_store.inc
	include asm/program_epilogue_win64.inc
defyx_program_epilogue ENDP

ALIGN 64
defyx_sshash_load PROC
	include asm/program_sshash_load.inc
defyx_sshash_load ENDP

defyx_sshash_prefetch PROC
	include asm/program_sshash_prefetch.inc
defyx_sshash_prefetch ENDP

defyx_sshash_end PROC
	nop
defyx_sshash_end ENDP

ALIGN 64
defyx_sshash_init PROC
	lea r8, [rbx+1]
	include asm/program_sshash_prefetch.inc
	imul r8, qword ptr [r0_mul]
	mov r9, qword ptr [r1_add]
	xor r9, r8
	mov r10, qword ptr [r2_add]
	xor r10, r8
	mov r11, qword ptr [r3_add]
	xor r11, r8
	mov r12, qword ptr [r4_add]
	xor r12, r8
	mov r13, qword ptr [r5_add]
	xor r13, r8
	mov r14, qword ptr [r6_add]
	xor r14, r8
	mov r15, qword ptr [r7_add]
	xor r15, r8
	jmp defyx_program_end
defyx_sshash_init ENDP

ALIGN 64
	include asm/program_sshash_constants.inc

ALIGN 64
defyx_program_end PROC
	nop
defyx_program_end ENDP

defyx_reciprocal_fast PROC
	include asm/defyx_reciprocal.inc
defyx_reciprocal_fast ENDP

_RANDOMX_JITX86_STATIC ENDS

ENDIF

END