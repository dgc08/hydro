	.file	"test.c"
	.intel_syntax noprefix
# GNU C17 (Ubuntu 11.4.0-1ubuntu1~22.04) version 11.4.0 (x86_64-linux-gnu)
#	compiled by GNU C version 11.4.0, GMP version 6.2.1, MPFR version 4.1.0, MPC version 1.2.1, isl version isl-0.24-GMP

# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
# options passed: -masm=intel -mtune=generic -march=x86-64 -Og -fasynchronous-unwind-tables -fstack-protector-strong -fstack-clash-protection -fcf-protection
	.text
	.globl	modi
	.type	modi, @function
modi:
.LFB23:
	.cfi_startproc
	endbr64	
# test.c:7:   glb = 2;
	mov	DWORD PTR glb[rip], 2	# glb,
# test.c:8: }
	ret	
	.cfi_endproc
.LFE23:
	.size	modi, .-modi
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"%d"
	.text
	.globl	main
	.type	main, @function
main:
.LFB24:
	.cfi_startproc
	endbr64	
	sub	rsp, 8	#,
	.cfi_def_cfa_offset 16
# test.c:12:   lc_main = lc_main + 1;
	mov	eax, DWORD PTR glb[rip]	# tmp91, glb
	add	eax, 1	# lc_main,
# test.c:13:   glb = lc_main;
	mov	DWORD PTR glb[rip], eax	# glb, lc_main
# test.c:14:   modi();
	mov	eax, 0	#,
	call	modi	#
# /usr/include/x86_64-linux-gnu/bits/stdio2.h:112:   return __printf_chk (__USE_FORTIFY_LEVEL - 1, __fmt, __va_arg_pack ());
	mov	edx, DWORD PTR glb[rip]	#, glb
	lea	rsi, .LC0[rip]	# tmp88,
	mov	edi, 1	#,
	mov	eax, 0	#,
	call	__printf_chk@PLT	#
# test.c:16: }
	mov	eax, 0	#,
	add	rsp, 8	#,
	.cfi_def_cfa_offset 8
	ret	
	.cfi_endproc
.LFE24:
	.size	main, .-main
	.globl	glb
	.bss
	.align 4
	.type	glb, @object
	.size	glb, 4
glb:
	.zero	4
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
