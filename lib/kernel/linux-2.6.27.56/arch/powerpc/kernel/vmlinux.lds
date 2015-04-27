

/*
 * Automatically generated C config: don't edit
 * Linux kernel version: 2.6.27.56
 * Tue Sep 23 18:50:58 2014
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
 * asm-generic/int-ll64.h
 *
 * Integer declarations for architectures which use "long long"
 * for 64-bit types.
 */
/*
 * These aren't exported outside the kernel to avoid name space clashes
 */
/* operations for longs and pointers */
/* Kdump kernel runs at 32 MB, change at your peril. */
/* How many bytes to reserve at zero for kdump. The reserve limit should
 * be greater or equal to the trampoline's end address.
 * Reserve to the end of the FWNMI area, see head_64.S */
/*
 * On PPC32 page size is 4K. For PPC64 we support either 4K or 64K software
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
/* Align . to a 8 byte boundary equals to maximum function alignment. */
/* The actual configuration determine if the init/exit sections
 * are handled as text/data or they can be discarded (which
 * often happens at runtime)
 */
/* .data section */
/* RODATA provided for backward compatibility.
 * All archs are supposed to use RO_DATA() */
/* .text section. Map to function alignment to avoid address changes
 * during second ld run in second ld pass when generating System.map */
/* sched.text is aling to function alignment to secure we have same
 * address even at second ld pass when generating System.map */
/* spinlock.text is aling to function alignment to secure we have same
 * address even at second ld pass when generating System.map */
/* Section used for early init (in .S files) */
/* init and exit section handling */
  /* DWARF debug sections.
		Symbols in the DWARF debugging sections are relative to
		the beginning of the section so we begin them at 0.  */
  /* Stabs debugging sections.  */
/* bytes per L1 cache line */
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
 /* Sections to be discarded. */
 /DISCARD/ : {
 *(.exitcall.exit)
 *(.exit.data) *(.cpuexit.data) *(.cpuexit.rodata) *(.memexit.data) *(.memexit.rodata)
 }
 . = 0xc0000000;
/*
 * Text, read only data and other permanent read-only sections
 */
 /* Text and gots */
 .text : AT(ADDR(.text) - (0xc0000000 -0x00000000)) {
  . = ALIGN(8);
  *(.text.head)
  _text = .;
  *(.text .fixup .text.init.refok .exit.text.refok __ftr_alt_*)
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
 . = ALIGN((4096)); .rodata : AT(ADDR(.rodata) - (0xc0000000 -0x00000000)) { __start_rodata = .; *(.rodata) *(.rodata.*) *(__vermagic) *(__markers_strings) } .rodata1 : AT(ADDR(.rodata1) - (0xc0000000 -0x00000000)) { *(.rodata1) } . = ALIGN(8); __bug_table : AT(ADDR(__bug_table) - (0xc0000000 -0x00000000)) { __start___bug_table = .; *(__bug_table) __stop___bug_table = .; } .pci_fixup : AT(ADDR(.pci_fixup) - (0xc0000000 -0x00000000)) { __start_pci_fixups_early = .; *(.pci_fixup_early) __end_pci_fixups_early = .; __start_pci_fixups_header = .; *(.pci_fixup_header) __end_pci_fixups_header = .; __start_pci_fixups_final = .; *(.pci_fixup_final) __end_pci_fixups_final = .; __start_pci_fixups_enable = .; *(.pci_fixup_enable) __end_pci_fixups_enable = .; __start_pci_fixups_resume = .; *(.pci_fixup_resume) __end_pci_fixups_resume = .; __start_pci_fixups_resume_early = .; *(.pci_fixup_resume_early) __end_pci_fixups_resume_early = .; __start_pci_fixups_suspend = .; *(.pci_fixup_suspend) __end_pci_fixups_suspend = .; } .builtin_fw : AT(ADDR(.builtin_fw) - (0xc0000000 -0x00000000)) { __start_builtin_fw = .; *(.builtin_fw) __end_builtin_fw = .; } .rio_route : AT(ADDR(.rio_route) - (0xc0000000 -0x00000000)) { __start_rio_route_ops = .; *(.rio_route_ops) __end_rio_route_ops = .; } __ksymtab : AT(ADDR(__ksymtab) - (0xc0000000 -0x00000000)) { __start___ksymtab = .; *(__ksymtab) __stop___ksymtab = .; } __ksymtab_gpl : AT(ADDR(__ksymtab_gpl) - (0xc0000000 -0x00000000)) { __start___ksymtab_gpl = .; *(__ksymtab_gpl) __stop___ksymtab_gpl = .; } __ksymtab_unused : AT(ADDR(__ksymtab_unused) - (0xc0000000 -0x00000000)) { __start___ksymtab_unused = .; *(__ksymtab_unused) __stop___ksymtab_unused = .; } __ksymtab_unused_gpl : AT(ADDR(__ksymtab_unused_gpl) - (0xc0000000 -0x00000000)) { __start___ksymtab_unused_gpl = .; *(__ksymtab_unused_gpl) __stop___ksymtab_unused_gpl = .; } __ksymtab_gpl_future : AT(ADDR(__ksymtab_gpl_future) - (0xc0000000 -0x00000000)) { __start___ksymtab_gpl_future = .; *(__ksymtab_gpl_future) __stop___ksymtab_gpl_future = .; } __kcrctab : AT(ADDR(__kcrctab) - (0xc0000000 -0x00000000)) { __start___kcrctab = .; *(__kcrctab) __stop___kcrctab = .; } __kcrctab_gpl : AT(ADDR(__kcrctab_gpl) - (0xc0000000 -0x00000000)) { __start___kcrctab_gpl = .; *(__kcrctab_gpl) __stop___kcrctab_gpl = .; } __kcrctab_unused : AT(ADDR(__kcrctab_unused) - (0xc0000000 -0x00000000)) { __start___kcrctab_unused = .; *(__kcrctab_unused) __stop___kcrctab_unused = .; } __kcrctab_unused_gpl : AT(ADDR(__kcrctab_unused_gpl) - (0xc0000000 -0x00000000)) { __start___kcrctab_unused_gpl = .; *(__kcrctab_unused_gpl) __stop___kcrctab_unused_gpl = .; } __kcrctab_gpl_future : AT(ADDR(__kcrctab_gpl_future) - (0xc0000000 -0x00000000)) { __start___kcrctab_gpl_future = .; *(__kcrctab_gpl_future) __stop___kcrctab_gpl_future = .; } __ksymtab_strings : AT(ADDR(__ksymtab_strings) - (0xc0000000 -0x00000000)) { *(__ksymtab_strings) } __init_rodata : AT(ADDR(__init_rodata) - (0xc0000000 -0x00000000)) { *(.ref.rodata) *(.devinit.rodata) *(.devexit.rodata) } __param : AT(ADDR(__param) - (0xc0000000 -0x00000000)) { __start___param = .; *(__param) __stop___param = .; . = ALIGN((4096)); __end_rodata = .; } . = ALIGN((4096));
 /* Exception & bug tables */
 __ex_table : AT(ADDR(__ex_table) - (0xc0000000 -0x00000000)) {
  __start___ex_table = .;
  *(__ex_table)
  __stop___ex_table = .;
 }
 .notes : AT(ADDR(.notes) - (0xc0000000 -0x00000000)) { __start_notes = .; *(.note.*) __stop_notes = .; } :kernel :notes
 /* The dummy segment contents for the bug workaround mentioned above
	   near PHDRS.  */
 .dummy : AT(ADDR(.dummy) - (0xc0000000 -0x00000000)) {
  LONG(0xf177)
 } :kernel :dummy
/*
 * Init sections discarded at runtime
 */
 . = ALIGN((1 << 12));
 __init_begin = .;
 .init.text : AT(ADDR(.init.text) - (0xc0000000 -0x00000000)) {
  _sinittext = .;
  *(.init.text) *(.cpuinit.text) *(.meminit.text)
  _einittext = .;
 } :kernel
 /* .exit.text is discarded at runtime, not link time,
	 * to deal with references from __bug_table
	 */
 .exit.text : AT(ADDR(.exit.text) - (0xc0000000 -0x00000000)) {
  *(.exit.text) *(.cpuexit.text) *(.memexit.text)
 }
 .init.data : AT(ADDR(.init.data) - (0xc0000000 -0x00000000)) {
  *(.init.data) *(.cpuinit.data) *(.cpuinit.rodata) *(.meminit.data) *(.meminit.rodata)
  __vtop_table_begin = .;
  *(.vtop_fixup);
  __vtop_table_end = .;
  __ptov_table_begin = .;
  *(.ptov_fixup);
  __ptov_table_end = .;
 }
 . = ALIGN(16);
 .init.setup : AT(ADDR(.init.setup) - (0xc0000000 -0x00000000)) {
  __setup_start = .;
  *(.init.setup)
  __setup_end = .;
 }
 .initcall.init : AT(ADDR(.initcall.init) - (0xc0000000 -0x00000000)) {
  __initcall_start = .;
  *(.initcallearly.init) __early_initcall_end = .; *(.initcall0.init) *(.initcall0s.init) *(.initcall1.init) *(.initcall1s.init) *(.initcall2.init) *(.initcall2s.init) *(.initcall3.init) *(.initcall3s.init) *(.initcall4.init) *(.initcall4s.init) *(.initcall5.init) *(.initcall5s.init) *(.initcallrootfs.init) *(.initcall6.init) *(.initcall6s.init) *(.initcall7.init) *(.initcall7s.init)
  __initcall_end = .;
  }
 .con_initcall.init : AT(ADDR(.con_initcall.init) - (0xc0000000 -0x00000000)) {
  __con_initcall_start = .;
  *(.con_initcall.init)
  __con_initcall_end = .;
 }
 .security_initcall.init : AT(ADDR(.security_initcall.init) - (0xc0000000 -0x00000000)) { __security_initcall_start = .; *(.security_initcall.init) __security_initcall_end = .; }
 . = ALIGN(8);
 __ftr_fixup : AT(ADDR(__ftr_fixup) - (0xc0000000 -0x00000000)) {
  __start___ftr_fixup = .;
  *(__ftr_fixup)
  __stop___ftr_fixup = .;
 }
 . = ALIGN(8);
 __lwsync_fixup : AT(ADDR(__lwsync_fixup) - (0xc0000000 -0x00000000)) {
  __start___lwsync_fixup = .;
  *(__lwsync_fixup)
  __stop___lwsync_fixup = .;
 }
 . = ALIGN((1 << 12));
 .init.ramfs : AT(ADDR(.init.ramfs) - (0xc0000000 -0x00000000)) {
  __initramfs_start = .;
  *(.init.ramfs)
  __initramfs_end = .;
 }
 . = ALIGN((1 << 12));
 .data.percpu : AT(ADDR(.data.percpu) - (0xc0000000 -0x00000000)) {
  __per_cpu_start = .;
  *(.data.percpu)
  *(.data.percpu.shared_aligned)
  __per_cpu_end = .;
 }
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
  *(.data) *(.data.init.refok) *(.ref.data) *(.devinit.data) *(.devexit.data) . = ALIGN(8); __start___markers = .; *(__markers) __stop___markers = .;
  *(.sdata)
  *(.got.plt) *(.got)
 }
 . = ALIGN((1 << 12));
 _edata = .;
 PROVIDE(edata = .);
 /* The initial task and kernel stack */
 . = ALIGN(8192);
 .data.init_task : AT(ADDR(.data.init_task) - (0xc0000000 -0x00000000)) {
  *(.data.init_task)
 }
 . = ALIGN((1 << 12));
 .data.page_aligned : AT(ADDR(.data.page_aligned) - (0xc0000000 -0x00000000)) {
  *(.data.page_aligned)
 }
 .data.cacheline_aligned : AT(ADDR(.data.cacheline_aligned) - (0xc0000000 -0x00000000)) {
  *(.data.cacheline_aligned)
 }
 . = ALIGN((1 << 5));
 .data.read_mostly : AT(ADDR(.data.read_mostly) - (0xc0000000 -0x00000000)) {
  *(.data.read_mostly)
 }
 . = ALIGN((1 << 12));
 .data_nosave : AT(ADDR(.data_nosave) - (0xc0000000 -0x00000000)) {
  __nosave_begin = .;
  *(.data.nosave)
  . = ALIGN((1 << 12));
  __nosave_end = .;
 }
/*
 * And finally the bss
 */
 .bss : AT(ADDR(.bss) - (0xc0000000 -0x00000000)) {
  __bss_start = .;
  *(.sbss) *(.scommon)
  *(.dynbss)
  *(.bss)
  *(COMMON)
  __bss_stop = .;
 }
 . = ALIGN((1 << 12));
 _end = . ;
 PROVIDE(end = .);
}
