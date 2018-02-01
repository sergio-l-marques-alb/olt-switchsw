#!/bin/sh

mv -v .svn .svn.tmp

echo "Applying patch..."
patch -p1 < tipc-4.4.30.patch
patch -p1 < ul200g.patch

#configura▒▒o do kernel
cp ul200g.config .config

#definir a toolchain
source /opt/fsl-1.9-p1010/environment-setup-ppce500v2-fsl-linux-gnuspe
unset LDFLAGS

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

cp arch/powerpc/boot/uImage ul200g.kernel.z
scripts/dtc/dtc -O dtb -o ul200g.dtb -b 0 -p 1024 arch/powerpc/boot/dts/fsl/ul200g.dts

echo "Reverting patch..."
patch -p1 -R < ul200g.patch
patch -p1 -R < tipc-4.4.30.patch
echo "Done!"

mv -v .svn.tmp .svn
