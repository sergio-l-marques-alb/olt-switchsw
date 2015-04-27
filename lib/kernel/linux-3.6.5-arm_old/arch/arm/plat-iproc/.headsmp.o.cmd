cmd_arch/arm/plat-iproc/headsmp.o := /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/buildroot/output/host/usr/bin/arm-linux-gcc -Wp,-MD,arch/arm/plat-iproc/.headsmp.o.d  -nostdinc -isystem /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/buildroot/host/usr/lib/gcc/arm-broadcom-linux-uclibcgnueabi/4.7.2/include -I/home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-iproc/include -Iarch/arm/plat-iproc/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork  -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -msoft-float -gdwarf-2        -c -o arch/arm/plat-iproc/headsmp.o arch/arm/plat-iproc/headsmp.S

source_arch/arm/plat-iproc/headsmp.o := arch/arm/plat-iproc/headsmp.S

deps_arch/arm/plat-iproc/headsmp.o := \
    $(wildcard include/config/smp.h) \
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
  include/linux/init.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/hotplug.h) \
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
  arch/arm/mach-iproc/include/mach/iproc_regs.h \
    $(wildcard include/config/mach/ns.h) \
    $(wildcard include/config/mach/hx4.h) \
    $(wildcard include/config/mach/kt2.h) \
    $(wildcard include/config/mach/hr2.h) \
    $(wildcard include/config/mach/nsp.h) \
    $(wildcard include/config/mach/iproc/p7.h) \
    $(wildcard include/config/mach/cygnus.h) \
    $(wildcard include/config/mach/gh.h) \
    $(wildcard include/config/iproc/gpio.h) \
    $(wildcard include/config/iproc/pwm.h) \
    $(wildcard include/config/iproc/ccb/wdt.h) \
    $(wildcard include/config/iproc/ccg/wdt.h) \
    $(wildcard include/config/iproc/sp805/wdt.h) \
  arch/arm/mach-iproc/include/mach/socregs_ing_open.h \
    $(wildcard include/config/ind/addr/base.h) \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/cp15.h \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/barrier.h \
    $(wildcard include/config/cpu/32v6k.h) \
    $(wildcard include/config/cpu/xsc3.h) \
    $(wildcard include/config/cpu/fa526.h) \
    $(wildcard include/config/arch/has/barriers.h) \
    $(wildcard include/config/arm/dma/mem/bufferable.h) \

arch/arm/plat-iproc/headsmp.o: $(deps_arch/arm/plat-iproc/headsmp.o)

$(deps_arch/arm/plat-iproc/headsmp.o):
