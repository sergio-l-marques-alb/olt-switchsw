

/*
 * Automatically generated C config: don't edit
 * Linux kernel version: 2.6.35
 * Tue Sep 23 22:56:19 2014
 */
/*
 * Copyright (C) 2001,2005 IBM Corporation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
/*
 * This is here because we used to use l64 for 64bit powerpc
 * and we don't want to impact user mode with our change to ll64
 * in the kernel.
 */
/*
 * asm-generic/int-ll64.h
 *
 * Integer declarations for architectures which use "long long"
 * for 64-bit types.
 */
/*
 * There seems to be no way of detecting this automatically from user
 * space, so 64 bit architectures should override this in their
 * bitsperlong.h. In particular, an architecture that supports
 * both 32 and 64 bit user space must not rely on CONFIG_64BIT
 * to decide it, but rather check a compiler provided macro.
 */
/*
 * FIXME: The check currently breaks x86-64 build, so it's
 * temporarily disabled. Please fix x86-64 and reenable
 */
/*
 * Copyright 2009 Freescale Semicondutor, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 * provides masks and opcode images for use by code generation, emulation
 * and for instructions that older assemblers might not know about
 */
/* Indirect stringification.  Doing two levels allows the parameter to be a
 * macro itself.  For example, compile with -DFOO=bar, __stringify(FOO)
 * converts to "bar".
 */
/* sorted alphabetically */




/* macros to insert fields into opcodes */







/*
 * Only use the larx hint bit on 64bit CPUs. e500v1/v2 based CPUs will treat a
 * larx with EH set as an illegal instruction.
 */






/* Deal with instructions that older assemblers aren't aware of */
/*
 * Define what the VSX XX1 form instructions will look like, then add
 * the 128 bit load store instructions based on that.
 */
/* operations for longs and pointers */
/*
 * If CONFIG_RELOCATABLE is enabled we can place the kdump kernel anywhere.
 * To keep enough space in the RMO for the first stage kernel on 64bit, we
 * place it at 64MB. If CONFIG_RELOCATABLE is not enabled we must place
 * the second stage at 32MB.
 */
/* How many bytes to reserve at zero for kdump. The reserve limit should
 * be greater or equal to the trampoline's end address.
 * Reserve to the end of the FWNMI area, see head_64.S */
/*
 * On regular PPC32 page size is 4K (but we support 4K/16K/64K/256K pages
 * on PPC44x). For PPC64 we support either 4K or 64K software
 * page size. When using 64K pages however, whether we are really supporting
 * 64K pages in HW or not is irrelevant to those definitions.
 */
/* We do define AT_SYSINFO_EHDR but don't use the gate mechanism */
/*
 * Subtle: (1 << PAGE_SHIFT) is an int, not an unsigned long. So if we
 * assign PAGE_MASK to a larger type it gets extended the way we want
 * (i.e. with 1s in the high bits)
 */
/*
 * KERNELBASE is the virtual address of the start of the kernel, it's often
 * the same as PAGE_OFFSET, but _might not be_.
 *
 * The kdump dump kernel is one example where KERNELBASE != PAGE_OFFSET.
 *
 * PAGE_OFFSET is the virtual address of the start of lowmem.
 *
 * PHYSICAL_START is the physical address of the start of the kernel.
 *
 * MEMORY_START is the physical address of the start of lowmem.
 *
 * KERNELBASE, PAGE_OFFSET, and PHYSICAL_START are all configurable on
 * ppc32 and based on how they are set we determine MEMORY_START.
 *
 * For the linear mapping the following equation should be true:
 * KERNELBASE - PAGE_OFFSET = PHYSICAL_START - MEMORY_START
 *
 * Also, KERNELBASE >= PAGE_OFFSET and PHYSICAL_START >= MEMORY_START
 *
 * There are two was to determine a physical address from a virtual one:
 * va = pa + PAGE_OFFSET - MEMORY_START
 * va = pa + KERNELBASE - PHYSICAL_START
 *
 * If you want to know something's offset from the start of the kernel you
 * should subtract KERNELBASE.
 *
 * If you want to test if something's a kernel address, use is_kernel_addr().
 */
/*
 * On Book-E parts we need __va to parse the device tree and we can't
 * determine MEMORY_START until then.  However we can determine PHYSICAL_START
 * from information at hand (program counter, TLB lookup).
 *
 * On non-Book-E PPC64 PAGE_OFFSET and MEMORY_START are constants so use
 * the other definitions for __va & __pa.
 */
/*
 * Unfortunately the PLT is in the BSS in the PPC32 ELF ABI,
 * and needs to be executable.  This means the whole heap ends
 * up being executable.
 */

/* align addr on a size boundary - adjust address up/down if needed */



/* align addr on a size boundary - adjust address up if needed */


/*
 * Don't compare things with KERNELBASE or PAGE_OFFSET to test for
 * "kernelness", use is_kernel_addr() - it should do what you want.
 */
/*
 * Helper macros to support writing architecture specific
 * linker scripts.
 *
 * A minimal linker scripts has following content:
 * [This is a sample, architectures may have special requiriements]
 *
 * OUTPUT_FORMAT(...)
 * OUTPUT_ARCH(...)
 * ENTRY(...)
 * SECTIONS
 * {
 *	. = START;
 *	__init_begin = .;
 *	HEAD_TEXT_SECTION
 *	INIT_TEXT_SECTION(PAGE_SIZE)
 *	INIT_DATA_SECTION(...)
 *	PERCPU(PAGE_SIZE)
 *	__init_end = .;
 *
 *	_stext = .;
 *	TEXT_SECTION = 0
 *	_etext = .;
 *
 *      _sdata = .;
 *	RO_DATA_SECTION(PAGE_SIZE)
 *	RW_DATA_SECTION(...)
 *	_edata = .;
 *
 *	EXCEPTION_TABLE(...)
 *	NOTES
 *
 *	BSS_SECTION(0, 0, 0)
 *	_end = .;
 *
 *	STABS_DEBUG
 *	DWARF_DEBUG
 *
 *	DISCARDS		// must be the last
 * }
 *
 * [__init_begin, __init_end] is the init section that may be freed after init
 * [_stext, _etext] is the text section
 * [_sdata, _edata] is the data section
 *
 * Some of the included output section have their own set of constants.
 * Examples are: [__initramfs_start, __initramfs_end] for initramfs and
 *               [__nosave_begin, __nosave_end] for the nosave data
 */
/* Align . to a 8 byte boundary equals to maximum function alignment. */
/*
 * Align to a 32 byte boundary equal to the
 * alignment gcc 4.5 uses for a struct
 */
/* The actual configuration determine if the init/exit sections
 * are handled as text/data or they can be discarded (which
 * often happens at runtime)
 */
/* .data section */
/*
 * Data section helpers
 */
/*
 * Read only Data
 */
/* RODATA & RO_DATA provided for backward compatibility.
 * All archs are supposed to use RO_DATA() */
/* .text section. Map to function alignment to avoid address changes
 * during second ld run in second ld pass when generating System.map */
/* sched.text is aling to function alignment to secure we have same
 * address even at second ld pass when generating System.map */
/* spinlock.text is aling to function alignment to secure we have same
 * address even at second ld pass when generating System.map */
/* Section used for early init (in .S files) */
/*
 * Exception table
 */
/*
 * Init task
 */
/* init and exit section handling */
/*
 * bss (Block Started by Symbol) - uninitialized data
 * zeroed during startup
 */
/*
 * DWARF debug sections.
 * Symbols in the DWARF debugging sections are relative to
 * the beginning of the section so we begin them at 0.
 */
  /* Stabs debugging sections.  */
/*
 * Default discarded sections.
 *
 * Some archs want to discard exit text/data at runtime rather than
 * link time due to cross-section references such as alt instructions,
 * bug table, eh_frame, etc.  DISCARDS must be the last of output
 * section definitions so that such archs put those in earlier section
 * definitions.
 */
/**
 * PERCPU_VADDR - define output section for percpu area
 * @vaddr: explicit base address (optional)
 * @phdr: destination PHDR (optional)
 *
 * Macro which expands to output section for percpu area.  If @vaddr
 * is not blank, it specifies explicit base address and all percpu
 * symbols will be offset from the given address.  If blank, @vaddr
 * always equals @laddr + LOAD_OFFSET.
 *
 * @phdr defines the output PHDR to use if not blank.  Be warned that
 * output PHDR is sticky.  If @phdr is specified, the next output
 * section in the linker script will go there too.  @phdr should have
 * a leading colon.
 *
 * Note that this macros defines __per_cpu_load as an absolute symbol.
 * If there is no need to put the percpu section at a predetermined
 * address, use PERCPU().
 */
/**
 * PERCPU - define output section for percpu area, simple version
 * @align: required alignment
 *
 * Align to @align and outputs output section for percpu area.  This
 * macro doesn't maniuplate @vaddr or @phdr and __per_cpu_load and
 * __per_cpu_start will be identical.
 *
 * This macro is equivalent to ALIGN(align); PERCPU_VADDR( , ) except
 * that __per_cpu_load is defined as a relative symbol against
 * .data..percpu which is required for relocatable x86_32
 * configuration.
 */
/*
 * Definition of the high level *_SECTION macros
 * They will fit only a subset of the architectures
 */
/*
 * Writeable data.
 * All sections are combined in a single .data section.
 * The sections following CONSTRUCTORS are arranged so their
 * typical alignment matches.
 * A cacheline is typical/always less than a PAGE_SIZE so
 * the sections that has this restriction (or similar)
 * is located before the ones requiring PAGE_SIZE alignment.
 * NOSAVE_DATA starts and ends with a PAGE_SIZE alignment which
 * matches the requirment of PAGE_ALIGNED_DATA.
 *
 * use 0 as page_align if page_aligned data is not used */
/* bytes per L1 cache line */
/* thread_info.h: PowerPC low-level thread information
 * adapted from the i386 version by Paul Mackerras
 *
 * Copyright (C) 2002  David Howells (dhowells@redhat.com)
 * - Incorporating suggestions made by Linus Torvalds and Dave Miller
 */
/* We have 8k stacks on ppc32 and 16k on ppc64 */
/*
 * thread information flag bit numbers
 */
/* as above, but as bit values */
/* Bits in local_flags */
/* Don't move TLF_NAPPING without adjusting the code in entry_32.S */
ENTRY(_stext)
PHDRS {
 kernel PT_LOAD FLAGS(7); /* RWX */
 notes PT_NOTE FLAGS(0);
 dummy PT_NOTE FLAGS(0);
 /* binutils < 2.18 has a bug that makes it misbehave when taking an
	   ELF file with all segments at load address 0 as input.  This
	   happens when running "strip" on vmlinux, because of the AT() magic
	   in this linker script.  People using GCC >= 4.2 won't run into
	   this problem, because the "build-id" support will put some data
	   into the "notes" segment (at a non-zero load address).

	   To work around this, we force some data into both the "dummy"
	   segment and the kernel segment, so the dummy segment will get a
	   non-zero load address.  It's not enough to always create the
	   "notes" segment, since if nothing gets assigned to it, its load
	   address will be zero.  */
}
OUTPUT_ARCH(powerpc:common)
jiffies = jiffies_64 + 4;
SECTIONS
{
 . = 0;
 reloc_start = .;
 . = 0xc0000000;
/*
 * Text, read only data and other permanent read-only sections
 */
 /* Text and gots */
 .text : AT(ADDR(.text) - (0xc0000000 -0x00000000)) {
  . = ALIGN(8);
  *(.head.text)
  _text = .;
  /* careful! __ftr_alt_* sections need to be close to .text */
  *(.text .fixup __ftr_alt_* .ref.text)
  . = ALIGN(8); __sched_text_start = .; *(.sched.text) __sched_text_end = .;
  . = ALIGN(8); __lock_text_start = .; *(.spinlock.text) __lock_text_end = .;
  . = ALIGN(8); __kprobes_text_start = .; *(.kprobes.text) __kprobes_text_end = .;
 
  *(.got1)
  __got2_start = .;
  *(.got2)
  __got2_end = .;
 } :kernel
 . = ALIGN((1 << 12));
 _etext = .;
 PROVIDE(etext = .);
 /* Read-only data */
 . = ALIGN((4096)); .rodata : AT(ADDR(.rodata) - (0xc0000000 -0x00000000)) { __start_rodata = .; *(.rodata) *(.rodata.*) *(__vermagic) *(__markers_strings) *(__tracepoints_strings) } .rodata1 : AT(ADDR(.rodata1) - (0xc0000000 -0x00000000)) { *(.rodata1) } . = ALIGN(8); __bug_table : AT(ADDR(__bug_table) - (0xc0000000 -0x00000000)) { __start___bug_table = .; *(__bug_table) __stop___bug_table = .; } .pci_fixup : AT(ADDR(.pci_fixup) - (0xc0000000 -0x00000000)) { __start_pci_fixups_early = .; *(.pci_fixup_early) __end_pci_fixups_early = .; __start_pci_fixups_header = .; *(.pci_fixup_header) __end_pci_fixups_header = .; __start_pci_fixups_final = .; *(.pci_fixup_final) __end_pci_fixups_final = .; __start_pci_fixups_enable = .; *(.pci_fixup_enable) __end_pci_fixups_enable = .; __start_pci_fixups_resume = .; *(.pci_fixup_resume) __end_pci_fixups_resume = .; __start_pci_fixups_resume_early = .; *(.pci_fixup_resume_early) __end_pci_fixups_resume_early = .; __start_pci_fixups_suspend = .; *(.pci_fixup_suspend) __end_pci_fixups_suspend = .; } .builtin_fw : AT(ADDR(.builtin_fw) - (0xc0000000 -0x00000000)) { __start_builtin_fw = .; *(.builtin_fw) __end_builtin_fw = .; } .rio_ops : AT(ADDR(.rio_ops) - (0xc0000000 -0x00000000)) { __start_rio_switch_ops = .; *(.rio_switch_ops) __end_rio_switch_ops = .; } __ksymtab : AT(ADDR(__ksymtab) - (0xc0000000 -0x00000000)) { __start___ksymtab = .; *(__ksymtab) __stop___ksymtab = .; } __ksymtab_gpl : AT(ADDR(__ksymtab_gpl) - (0xc0000000 -0x00000000)) { __start___ksymtab_gpl = .; *(__ksymtab_gpl) __stop___ksymtab_gpl = .; } __ksymtab_unused : AT(ADDR(__ksymtab_unused) - (0xc0000000 -0x00000000)) { __start___ksymtab_unused = .; *(__ksymtab_unused) __stop___ksymtab_unused = .; } __ksymtab_unused_gpl : AT(ADDR(__ksymtab_unused_gpl) - (0xc0000000 -0x00000000)) { __start___ksymtab_unused_gpl = .; *(__ksymtab_unused_gpl) __stop___ksymtab_unused_gpl = .; } __ksymtab_gpl_future : AT(ADDR(__ksymtab_gpl_future) - (0xc0000000 -0x00000000)) { __start___ksymtab_gpl_future = .; *(__ksymtab_gpl_future) __stop___ksymtab_gpl_future = .; } __kcrctab : AT(ADDR(__kcrctab) - (0xc0000000 -0x00000000)) { __start___kcrctab = .; *(__kcrctab) __stop___kcrctab = .; } __kcrctab_gpl : AT(ADDR(__kcrctab_gpl) - (0xc0000000 -0x00000000)) { __start___kcrctab_gpl = .; *(__kcrctab_gpl) __stop___kcrctab_gpl = .; } __kcrctab_unused : AT(ADDR(__kcrctab_unused) - (0xc0000000 -0x00000000)) { __start___kcrctab_unused = .; *(__kcrctab_unused) __stop___kcrctab_unused = .; } __kcrctab_unused_gpl : AT(ADDR(__kcrctab_unused_gpl) - (0xc0000000 -0x00000000)) { __start___kcrctab_unused_gpl = .; *(__kcrctab_unused_gpl) __stop___kcrctab_unused_gpl = .; } __kcrctab_gpl_future : AT(ADDR(__kcrctab_gpl_future) - (0xc0000000 -0x00000000)) { __start___kcrctab_gpl_future = .; *(__kcrctab_gpl_future) __stop___kcrctab_gpl_future = .; } __ksymtab_strings : AT(ADDR(__ksymtab_strings) - (0xc0000000 -0x00000000)) { *(__ksymtab_strings) } __init_rodata : AT(ADDR(__init_rodata) - (0xc0000000 -0x00000000)) { *(.ref.rodata) *(.devinit.rodata) *(.devexit.rodata) } __param : AT(ADDR(__param) - (0xc0000000 -0x00000000)) { __start___param = .; *(__param) __stop___param = .; . = ALIGN((4096)); __end_rodata = .; } . = ALIGN((4096));
 . = ALIGN(0); __ex_table : AT(ADDR(__ex_table) - (0xc0000000 -0x00000000)) { __start___ex_table = .; *(__ex_table) __stop___ex_table = .; }
 .notes : AT(ADDR(.notes) - (0xc0000000 -0x00000000)) { __start_notes = .; *(.note.*) __stop_notes = .; } :kernel :notes
 /* The dummy segment contents for the bug workaround mentioned above
	   near PHDRS.  */
 .dummy : AT(ADDR(.dummy) - (0xc0000000 -0x00000000)) {
  LONG(0)
  LONG(0)
  LONG(0)
 } :kernel :dummy
/*
 * Init sections discarded at runtime
 */
 . = ALIGN((1 << 12));
 __init_begin = .;
 . = ALIGN((1 << 12)); .init.text : AT(ADDR(.init.text) - (0xc0000000 -0x00000000)) { _sinittext = .; *(.init.text) *(.cpuinit.text) *(.meminit.text) _einittext = .; } :kernel
 /* .exit.text is discarded at runtime, not link time,
	 * to deal with references from __bug_table
	 */
 .exit.text : AT(ADDR(.exit.text) - (0xc0000000 -0x00000000)) {
  *(.exit.text) *(.cpuexit.text) *(.memexit.text)
 }
 .init.data : AT(ADDR(.init.data) - (0xc0000000 -0x00000000)) {
  *(.init.data) *(.cpuinit.data) *(.meminit.data) . = ALIGN(8); __ctors_start = .; *(.ctors) __ctors_end = .; *(.init.rodata) *(.cpuinit.rodata) *(.meminit.rodata)
  __vtop_table_begin = .;
  *(.vtop_fixup);
  __vtop_table_end = .;
  __ptov_table_begin = .;
  *(.ptov_fixup);
  __ptov_table_end = .;
 }
 .init.setup : AT(ADDR(.init.setup) - (0xc0000000 -0x00000000)) {
  . = ALIGN(16); __setup_start = .; *(.init.setup) __setup_end = .;
 }
 .initcall.init : AT(ADDR(.initcall.init) - (0xc0000000 -0x00000000)) {
  __initcall_start = .; *(.initcallearly.init) __early_initcall_end = .; *(.initcall0.init) *(.initcall0s.init) *(.initcall1.init) *(.initcall1s.init) *(.initcall2.init) *(.initcall2s.init) *(.initcall3.init) *(.initcall3s.init) *(.initcall4.init) *(.initcall4s.init) *(.initcall5.init) *(.initcall5s.init) *(.initcallrootfs.init) *(.initcall6.init) *(.initcall6s.init) *(.initcall7.init) *(.initcall7s.init) __initcall_end = .;
 }
 .con_initcall.init : AT(ADDR(.con_initcall.init) - (0xc0000000 -0x00000000)) {
  __con_initcall_start = .; *(.con_initcall.init) __con_initcall_end = .;
 }
 .security_initcall.init : AT(ADDR(.security_initcall.init) - (0xc0000000 -0x00000000)) { __security_initcall_start = .; *(.security_initcall.init) __security_initcall_end = .; }
 . = ALIGN(8);
 __ftr_fixup : AT(ADDR(__ftr_fixup) - (0xc0000000 -0x00000000)) {
  __start___ftr_fixup = .;
  *(__ftr_fixup)
  __stop___ftr_fixup = .;
 }
 . = ALIGN(8);
 __mmu_ftr_fixup : AT(ADDR(__mmu_ftr_fixup) - (0xc0000000 -0x00000000)) {
  __start___mmu_ftr_fixup = .;
  *(__mmu_ftr_fixup)
  __stop___mmu_ftr_fixup = .;
 }
 . = ALIGN(8);
 __lwsync_fixup : AT(ADDR(__lwsync_fixup) - (0xc0000000 -0x00000000)) {
  __start___lwsync_fixup = .;
  *(__lwsync_fixup)
  __stop___lwsync_fixup = .;
 }
 .init.ramfs : AT(ADDR(.init.ramfs) - (0xc0000000 -0x00000000)) {
  . = ALIGN((1 << 12)); __initramfs_start = .; *(.init.ramfs) __initramfs_end = .;
 }
 . = ALIGN((1 << 12)); .data..percpu : AT(ADDR(.data..percpu) - (0xc0000000 -0x00000000)) { __per_cpu_load = .; __per_cpu_start = .; *(.data..percpu..first) *(.data..percpu..page_aligned) *(.data..percpu) *(.data..percpu..shared_aligned) __per_cpu_end = .; }
 . = ALIGN(8);
 .machine.desc : AT(ADDR(.machine.desc) - (0xc0000000 -0x00000000)) {
  __machine_desc_start = . ;
  *(.machine.desc)
  __machine_desc_end = . ;
 }
 /* freed after init ends here */
 . = ALIGN((1 << 12));
 __init_end = .;
/*
 * And now the various read/write data
 */
 . = ALIGN((1 << 12));
 _sdata = .;
 .data : AT(ADDR(.data) - (0xc0000000 -0x00000000)) {
  *(.data) *(.ref.data) *(.devinit.data) *(.devexit.data) . = ALIGN(8); __start___markers = .; *(__markers) __stop___markers = .; . = ALIGN(32); __start___tracepoints = .; *(__tracepoints) __stop___tracepoints = .; . = ALIGN(8); __start___verbose = .; *(__verbose) __stop___verbose = .; . = ALIGN(32); . = ALIGN(32);
  *(.sdata)
  *(.got.plt) *(.got)
 }
 /* The initial task and kernel stack */
 . = ALIGN((1 << 13)); .data..init_task : AT(ADDR(.data..init_task) - (0xc0000000 -0x00000000)) { . = ALIGN((1 << 13)); *(.data..init_task) }
 .data..page_aligned : AT(ADDR(.data..page_aligned) - (0xc0000000 -0x00000000)) {
  . = ALIGN((1 << 12)); *(.data..page_aligned)
 }
 .data..cacheline_aligned : AT(ADDR(.data..cacheline_aligned) - (0xc0000000 -0x00000000)) {
  . = ALIGN((1 << 5)); *(.data..cacheline_aligned)
 }
 .data..read_mostly : AT(ADDR(.data..read_mostly) - (0xc0000000 -0x00000000)) {
  . = ALIGN((1 << 5)); *(.data..read_mostly)
 }
 . = ALIGN((1 << 12));
 .data_nosave : AT(ADDR(.data_nosave) - (0xc0000000 -0x00000000)) {
  . = ALIGN((1 << 12)); __nosave_begin = .; *(.data..nosave) . = ALIGN((1 << 12)); __nosave_end = .;
 }
 . = ALIGN((1 << 12));
 _edata = .;
 PROVIDE(edata = .);
/*
 * And finally the bss
 */
 . = ALIGN(0); __bss_start = .; . = ALIGN(0); .sbss : AT(ADDR(.sbss) - (0xc0000000 -0x00000000)) { *(.sbss) *(.scommon) } . = ALIGN(0); .bss : AT(ADDR(.bss) - (0xc0000000 -0x00000000)) { *(.bss..page_aligned) *(.dynbss) *(.bss) *(COMMON) } . = ALIGN(0); __bss_stop = .;
 . = ALIGN((1 << 12));
 _end = . ;
 PROVIDE(end = .);
 /* Sections to be discarded. */
 /DISCARD/ : { *(.exit.text) *(.cpuexit.text) *(.memexit.text) *(.exit.data) *(.cpuexit.data) *(.cpuexit.rodata) *(.memexit.data) *(.memexit.rodata) *(.exitcall.exit) *(.discard) }
}
