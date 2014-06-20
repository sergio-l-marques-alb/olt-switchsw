CROSS_COMPILER=/opt/broadcom/bin/arm-broadcom-linux-uclibcgnueabi-

KFLAG_INCLD=/opt/broadcom/lib/gcc/arm-broadcom-linux-uclibcgnueabi/4.7.2/include

KFLAGS="-D__LINUX_ARM_ARCH__=7 -D__KERNEL__ -isystem $KFLAG_INCLD -Wall -Wstrict-prototypes -Wno-trigraphs -Os -fno-strict-aliasing -fno-common -marm -mabi=aapcs-linux -fno-pic -pipe -msoft-float -ffreestanding -march=armv7-a -mfpu=vfp -mfloat-abi=softfp -fomit-frame-pointer -g -fno-stack-protector -Wdeclaration-after-statement -Wno-pointer-sign -mlong-calls"

${CROSS_COMPILER}gcc ${KFLAGS} -o io io.c
