cmd_firmware/tigon/tg3.bin.gen.o := /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/buildroot/output/host/usr/bin/arm-linux-gcc -Wp,-MD,firmware/tigon/.tg3.bin.gen.o.d  -nostdinc -isystem /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/buildroot/host/usr/lib/gcc/arm-broadcom-linux-uclibcgnueabi/4.7.2/include -I/home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-iproc/include -Iarch/arm/plat-iproc/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork  -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -msoft-float -gdwarf-2        -c -o firmware/tigon/tg3.bin.gen.o firmware/tigon/tg3.bin.gen.S

source_firmware/tigon/tg3.bin.gen.o := firmware/tigon/tg3.bin.gen.S

deps_firmware/tigon/tg3.bin.gen.o := \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \

firmware/tigon/tg3.bin.gen.o: $(deps_firmware/tigon/tg3.bin.gen.o)

$(deps_firmware/tigon/tg3.bin.gen.o):
