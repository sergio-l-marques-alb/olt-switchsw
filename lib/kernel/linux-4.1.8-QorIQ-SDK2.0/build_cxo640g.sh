#!/bin/sh

mv -v .svn .svn.tmp

echo "Applying patch..."
patch -p1 < tipc-4.4.30.patch
patch -p1 < kernel-4.1-cxo640.patch

#configura▒▒o do kernel
cp kernel-4.1-cxo640.config .config

#definir a toolchain
CROSS_COMPILE=/opt/fsl/1.2/sysroots/i686-fslsdk-linux/usr/bin/ppce500mc-fsl-linux/powerpc-fsl-linux-
DEVDIR=/opt/fsl/1.2/sysroots/i686-fslsdk-linux/usr
CC='${CROSS-COMPILE}gcc'
AS='${CROSS_COMPILE}as'
LD='${CROSS_COMPILE}ld'
AR='${CROSS_COMPILE}ar'
RANLIB='${CROSS_COMPILE}ranlib'
CXX='${CROSS_COMPILE}g++'
GDB='${CROSS_COMPILE}gdb'
NM='${CROSS_COMPILE}nm'
ARCH=powerpc
CONFIGURE_FLAGS="--target=powerpc-fsl-linux --host=powerpc-fsl-linux --build=i686-linux --with-libtool-sysroot=/opt/fsl/1.2/sysroots/ppce500mc-fsl-linux"
CFLAGS=" -m32 -mhard-float  -mcpu=e500mc --sysroot=/opt/fsl/1.2/sysroots/ppce500mc-fsl-linux"
CXXFLAGS=" -m32 -mhard-float  -mcpu=e500mc --sysroot=/opt/fsl/1.2/sysroots/ppce500mc-fsl-linux"
LDFLAGS=" --sysroot=/opt/fsl/1.2/sysroots/ppce500mc-fsl-linux"
CPPFLAGS=" -m32 -mhard-float  -mcpu=e500mc --sysroot=/opt/fsl/1.2/sysroots/ppce500mc-fsl-linux"
export CC AS CPPFLAGS LD  CROSS_COMPILE ARCH RANLIB NM CONFIGURE_FLAGS CFLAGS LDFLAGS CXXFLAGS

#Clean
#make clean

#compilar
make -j`grep -c '^processor' /proc/cpuinfo` uImage

#compilar os modulos de kernel
make modules

#instalar os modulos de kernel
make modules_install INSTALL_MOD_PATH=$PWD/rootfs

#instalar os include headers do linux
make headers_install ARCH=$ARCH INSTALL_HDR_PATH=$PWD/linux-inc

cp arch/powerpc/boot/uImage cxo640g.z
scripts/dtc/dtc -O dtb -o cxo640g.dtb -b 0 -p 1024 arch/powerpc/boot/dts/fsl/cxo640.dts

echo "Reverting patch..."
patch -p1 -R < kernel-4.1-cxo640.patch
patch -p1 -R < tipc-4.4.30.patch
echo "Done!"

mv -v .svn.tmp .svn
