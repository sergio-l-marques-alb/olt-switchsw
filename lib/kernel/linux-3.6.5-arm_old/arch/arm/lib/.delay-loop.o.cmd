cmd_arch/arm/lib/delay-loop.o := /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/buildroot/output/host/usr/bin/arm-linux-gcc -Wp,-MD,arch/arm/lib/.delay-loop.o.d  -nostdinc -isystem /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/buildroot/host/usr/lib/gcc/arm-broadcom-linux-uclibcgnueabi/4.7.2/include -I/home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-iproc/include -Iarch/arm/plat-iproc/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork  -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -msoft-float -gdwarf-2        -c -o arch/arm/lib/delay-loop.o arch/arm/lib/delay-loop.S

source_arch/arm/lib/delay-loop.o := arch/arm/lib/delay-loop.S

deps_arch/arm/lib/delay-loop.o := \
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
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/delay.h \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/memory.h \
    $(wildcard include/config/need/mach/memory/h.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/page/offset.h) \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/dram/size.h) \
    $(wildcard include/config/dram/base.h) \
    $(wildcard include/config/have/tcm.h) \
    $(wildcard include/config/arm/patch/phys/virt.h) \
    $(wildcard include/config/phys/offset.h) \
  include/linux/const.h \
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
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/param.h \
    $(wildcard include/config/hz.h) \

arch/arm/lib/delay-loop.o: $(deps_arch/arm/lib/delay-loop.o)

$(deps_arch/arm/lib/delay-loop.o):
