#!/bin/bash

mv -v .svn .svn.tmp

export DEVDIR=/opt/eldk/usr
export CROSS=$DEVDIR/bin/ppc_85xxDP-
export ARCH=powerpc
export CROSS_COMPILE=${CROSS}

patch -p1 -R < tg16g.patch
patch -p1 < cxo640_uP2.patch
rm -rf .config
cp config_cxo640g_uP2 .config

make uImage -j `grep -c '^processor' /proc/cpuinfo`
make modules -j `grep -c '^processor' /proc/cpuinfo`
make modules_install INSTALL_MOD_PATH=$PWD/rootfs
make headers_install INSTALL_HDR_PATH=$PWD/linux-inc
scripts/dtc/dtc -O dtb -o cxo640g_uP2.dtb -b 0 -p 1024 ./cxo640g_uP2.dts
rm -rf ./cxo640g_uP2.z; cp arch/powerpc/boot/uImage ./cxo640g_uP2.z

mv -v .svn.tmp .svn

