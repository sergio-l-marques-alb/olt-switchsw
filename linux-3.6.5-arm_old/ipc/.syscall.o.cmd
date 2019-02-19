cmd_ipc/syscall.o := /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/buildroot/output/host/usr/bin/arm-linux-gcc -Wp,-MD,ipc/.syscall.o.d  -nostdinc -isystem /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/buildroot/host/usr/lib/gcc/arm-broadcom-linux-uclibcgnueabi/4.7.2/include -I/home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-iproc/include -Iarch/arm/plat-iproc/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -O2 -marm -fno-dwarf2-cfi-asm -fno-omit-frame-pointer -mapcs -mno-sched-prolog -mabi=aapcs-linux -mno-thumb-interwork -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -g -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(syscall)"  -D"KBUILD_MODNAME=KBUILD_STR(syscall)" -c -o ipc/syscall.o ipc/syscall.c

source_ipc/syscall.o := ipc/syscall.c

deps_ipc/syscall.o := \
  include/linux/unistd.h \
  /home/devtools/dev-bcm-ldk/3.4.7-RC4/iproc/kernel/linux-3.6.5/arch/arm/include/asm/unistd.h \
    $(wildcard include/config/aeabi.h) \
    $(wildcard include/config/oabi/compat.h) \

ipc/syscall.o: $(deps_ipc/syscall.o)

$(deps_ipc/syscall.o):
