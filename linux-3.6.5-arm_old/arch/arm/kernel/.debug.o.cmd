cmd_arch/arm/kernel/debug.o := /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/buildroot/output/host/usr/bin/arm-linux-gcc -Wp,-MD,arch/arm/kernel/.debug.o.d  -nostdinc -isystem /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/buildroot/host/usr/lib/gcc/arm-broadcom-linux-uclibcgnueabi/4.7.2/include -I/home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-iproc/include -Iarch/arm/plat-iproc/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork  -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -msoft-float -gdwarf-2        -c -o arch/arm/kernel/debug.o arch/arm/kernel/debug.S

source_arch/arm/kernel/debug.o := arch/arm/kernel/debug.S

deps_arch/arm/kernel/debug.o := \
    $(wildcard include/config/debug/icedcc.h) \
    $(wildcard include/config/cpu/v6.h) \
    $(wildcard include/config/cpu/v6k.h) \
    $(wildcard include/config/cpu/v7.h) \
    $(wildcard include/config/cpu/xscale.h) \
    $(wildcard include/config/debug/semihosting.h) \
    $(wildcard include/config/mmu.h) \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/linkage.h \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/assembler.h \
    $(wildcard include/config/cpu/feroceon.h) \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/cpu/use/domains.h) \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/cpu/endian/be8.h) \
    $(wildcard include/config/arm/thumb.h) \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/hwcap.h \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/domain.h \
    $(wildcard include/config/io/36.h) \
  arch/arm/plat-iproc/include/mach/debug-macro.S \
    $(wildcard include/config/mach/cygnus.h) \
  arch/arm/mach-iproc/include/mach/io_map.h \
  arch/arm/plat-iproc/include/mach/memory.h \
    $(wildcard include/config/bcm/ram/base.h) \
    $(wildcard include/config/bcm/ram/start/reserved/size.h) \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/pgtable.h \
    $(wildcard include/config/arm/lpae.h) \
    $(wildcard include/config/arm/dma/mem/bufferable.h) \
    $(wildcard include/config/highpte.h) \
  include/linux/const.h \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/proc-fns.h \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/glue-proc.h \
    $(wildcard include/config/cpu/arm7tdmi.h) \
    $(wildcard include/config/cpu/arm720t.h) \
    $(wildcard include/config/cpu/arm740t.h) \
    $(wildcard include/config/cpu/arm9tdmi.h) \
    $(wildcard include/config/cpu/arm920t.h) \
    $(wildcard include/config/cpu/arm922t.h) \
    $(wildcard include/config/cpu/fa526.h) \
    $(wildcard include/config/cpu/arm925t.h) \
    $(wildcard include/config/cpu/arm926t.h) \
    $(wildcard include/config/cpu/arm940t.h) \
    $(wildcard include/config/cpu/arm946e.h) \
    $(wildcard include/config/cpu/sa110.h) \
    $(wildcard include/config/cpu/sa1100.h) \
    $(wildcard include/config/cpu/arm1020.h) \
    $(wildcard include/config/cpu/arm1020e.h) \
    $(wildcard include/config/cpu/arm1022.h) \
    $(wildcard include/config/cpu/arm1026.h) \
    $(wildcard include/config/cpu/xsc3.h) \
    $(wildcard include/config/cpu/mohawk.h) \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/glue.h \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/page.h \
    $(wildcard include/config/cpu/copy/v4wt.h) \
    $(wildcard include/config/cpu/copy/v4wb.h) \
    $(wildcard include/config/cpu/copy/feroceon.h) \
    $(wildcard include/config/cpu/copy/fa.h) \
    $(wildcard include/config/cpu/copy/v6.h) \
    $(wildcard include/config/have/arch/pfn/valid.h) \
  include/asm-generic/getorder.h \
  include/asm-generic/pgtable-nopud.h \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/memory.h \
    $(wildcard include/config/need/mach/memory/h.h) \
    $(wildcard include/config/page/offset.h) \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/dram/size.h) \
    $(wildcard include/config/dram/base.h) \
    $(wildcard include/config/have/tcm.h) \
    $(wildcard include/config/arm/patch/phys/virt.h) \
    $(wildcard include/config/phys/offset.h) \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/types.h \
  include/asm-generic/int-ll64.h \
  arch/arm/include/generated/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/linux/sizes.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
    $(wildcard include/config/sparsemem.h) \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/pgtable-hwdef.h \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/pgtable-2level-hwdef.h \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/pgtable-2level.h \
  arch/arm/mach-iproc/include/mach/iproc_regs.h \
    $(wildcard include/config/mach/ns.h) \
    $(wildcard include/config/mach/hx4.h) \
    $(wildcard include/config/mach/kt2.h) \
    $(wildcard include/config/mach/hr2.h) \
    $(wildcard include/config/mach/nsp.h) \
    $(wildcard include/config/mach/iproc/p7.h) \
    $(wildcard include/config/mach/gh.h) \
    $(wildcard include/config/iproc/gpio.h) \
    $(wildcard include/config/iproc/pwm.h) \
    $(wildcard include/config/iproc/ccb/wdt.h) \
    $(wildcard include/config/iproc/ccg/wdt.h) \
    $(wildcard include/config/iproc/sp805/wdt.h) \
  arch/arm/mach-iproc/include/mach/socregs_ing_open.h \
    $(wildcard include/config/ind/addr/base.h) \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/hardware/debug-8250.S \
  include/linux/serial_reg.h \

arch/arm/kernel/debug.o: $(deps_arch/arm/kernel/debug.o)

$(deps_arch/arm/kernel/debug.o):
