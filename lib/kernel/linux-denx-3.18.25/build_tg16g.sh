#!/bin/bash

export DEVDIR=/opt/freescale/usr/local/gcc-4.0.2-glibc-2.3.6-nptl-2/powerpc-e300c3-linux
export CROSS=$DEVDIR/bin/powerpc-e300c3-linux-
export ARCH=powerpc
export CROSS_COMPILE=${CROSS}

make clean
make uImage -j `grep -c '^processor' /proc/cpuinfo`
make modules -j `grep -c '^processor' /proc/cpuinfo`

#instalar os modulos de kernel
make modules_install INSTALL_MOD_PATH=$PWD/rootfs

#instalar os include headers do linux
make headers_install ARCH=$ARCH INSTALL_HDR_PATH=$PWD/linux-inc

#rm -rf tg16g.kernel.z
cp arch/powerpc/boot/uImage tg16g.kernel.z
scripts/dtc/dtc -O dtb -o tg16g.dtb -b 0 -p 1024 tg16g.dts

