/*
 * Copyright (C) Paul Mackerras 1997.
 *
 * Adapted for 64 bit LE PowerPC by Andrew Tauferner
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 */

#include "ppc_asm.h"

RELA = 7
RELACOUNT = 0x6ffffff9

	.text
	/* A procedure descriptor used when booting this as a COFF file.
	 * When making COFF, this comes first in the link and we're
	 * linked at 0x500000.
	 */
	.globl	_zimage_start_opd
_zimage_start_opd:
	.long	0x500000, 0, 0, 0

p_start:	.long	_start
p_etext:	.long	_etext
p_bss_start:	.long	__bss_start
p_end:		.long	_end

	.weak	_platform_stack_top
p_pstack:	.long	_platform_stack_top

	.weak	_zimage_start
	.globl	_zimage_start
_zimage_start:
	.globl	_zimage_start_lib
_zimage_start_lib:
	/* Work out the offset between the address we were linked at
	   and the address where we're running. */
	bl	.+4
p_base:	mflr	r10		/* r10 now points to runtime addr of p_base */
	/* Save the prom pointer at p_prom. */
	std	r5,(p_prom-p_base)(r10)

	/* Set r2 to the TOC. */
	ld	r2,(p_toc-p_base)(r10)
	add	r2,r2,r10

	/* Grab the link address of the dynamic section in r11. */
	ld	r11,-32768(r2)
	cmpwi	r11,0
	beq	3f              /* if not linked -pie then no dynamic section */

	ld	r11,(p_dyn-p_base)(r10)
	add	r11,r11,r10
	ld	r9,(p_rela-p_base)(r10)
	add	r9,r9,r10

	li	r7,0
	li	r8,0
9:	ld	r6,0(r11)       /* get tag */
	cmpdi	r6,0
	beq	12f              /* end of list */
	cmpdi	r6,RELA
	bne	10f
	ld	r7,8(r11)       /* get RELA pointer in r7 */
	b	11f
10:	addis	r6,r6,(-RELACOUNT)@ha
	cmpdi	r6,RELACOUNT@l
	bne	11f
	ld	r8,8(r11)       /* get RELACOUNT value in r8 */
11:	addi	r11,r11,16
	b	9b
12:
	cmpdi	r7,0            /* check we have both RELA and RELACOUNT */
	cmpdi	cr1,r8,0
	beq	3f
	beq	cr1,3f

	/* Calcuate the runtime offset. */
	subf	r7,r7,r9

	/* Run through the list of relocations and process the
	 * R_PPC64_RELATIVE ones. */
	mtctr	r8
13:	ld	r0,8(r9)        /* ELF64_R_TYPE(reloc->r_info) */
	cmpdi	r0,22           /* R_PPC64_RELATIVE */
	bne	3f
	ld	r6,0(r9)        /* reloc->r_offset */
	ld	r0,16(r9)       /* reloc->r_addend */
	add	r0,r0,r7
	stdx	r0,r7,r6
	addi	r9,r9,24
	bdnz	13b

	/* Do a cache flush for our text, in case the loader didn't */
3:	ld	r9,p_start-p_base(r10)	/* note: these are relocated now */
	ld	r8,p_etext-p_base(r10)
4:	dcbf	r0,r9
	icbi	r0,r9
	addi	r9,r9,0x20
	cmpld	cr0,r9,r8
	blt	4b
	sync
	isync

	/* Clear the BSS */
	ld	r9,p_bss_start-p_base(r10)
	ld	r8,p_end-p_base(r10)
	li	r0,0
5:	std	r0,0(r9)
	addi	r9,r9,8
	cmpld	cr0,r9,r8
	blt	5b

	/* Possibly set up a custom stack */
	ld	r8,p_pstack-p_base(r10)
	cmpdi	r8,0
	beq	6f
	ld	r1,0(r8)
	li	r0,0
	stdu	r0,-16(r1)	/* establish a stack frame */
6:
	/* Call platform_init() */
	bl	platform_init

	/* Call start */
	b	start
