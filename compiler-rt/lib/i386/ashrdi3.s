// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

// di_int __ashrdi3(di_int input, int count);

#ifdef __i386__
#ifdef __SSE2__

.text
.align 4
.globl ___ashrdi3
___ashrdi3:
	movd	  12(%esp),		%xmm2	// Load count
	movl	   8(%esp),		%eax
#ifndef TRUST_CALLERS_USE_64_BIT_STORES
	movd	   4(%esp),		%xmm0
	movd	   8(%esp),		%xmm1
	punpckldq	%xmm1,		%xmm0	// Load input
#else
	movq	   4(%esp),		%xmm0	// Load input
#endif

	psrlq		%xmm2,		%xmm0	// unsigned shift input by count
	
	testl		%eax,		%eax	// check the sign-bit of the input
	jns			1f					// early out for positive inputs
	
	// If the input is negative, we need to construct the shifted sign bit
	// to or into the result, as xmm does not have a signed right shift.
	pcmpeqb		%xmm1,		%xmm1	// -1ULL
	psrlq		$58,		%xmm1	// 0x3f
	pandn		%xmm1,		%xmm2	// 63 - count
	pcmpeqb		%xmm1,		%xmm1	// -1ULL
	psubq		%xmm1,		%xmm2	// 64 - count
	psllq		%xmm2,		%xmm1	// -1 << (64 - count) = leading sign bits
	por			%xmm1,		%xmm0
	
	// Move the result back to the general purpose registers and return
1:	movd		%xmm0,		%eax
	psrlq		$32,		%xmm0
	movd		%xmm0,		%edx
	ret

#else // Use GPRs instead of SSE2 instructions, if they aren't available.

.text
.align 4
.globl ___ashrdi3
___ashrdi3:
	movl	  12(%esp),		%ecx	// Load count
	movl	   8(%esp),		%edx	// Load high
	movl	   4(%esp),		%eax	// Load low
	
	testl		$0x20,		%ecx	// If count >= 32
	jnz			2f					//    goto 2
	testl		$0x1f,		%ecx	// If count == 0
	jz			1f					//    goto 1
	
	pushl		%ebx
	movl		%edx,		%ebx	// copy high
	shrl		%cl,		%eax	// right shift low by count
	sarl		%cl,		%edx	// right shift high by count
	neg			%cl
	shll		%cl,		%ebx	// left shift high by 32 - count
	orl			%ebx,		%eax	// or the result into the low word
	popl		%ebx
1:	ret
	
2:	movl		%edx,		%eax	// Move high to low
	sarl		$31,		%edx	// clear high
	sarl		%cl,		%eax	// shift low by count - 32
	ret
	
#endif // __SSE2__
#endif // __i386__
