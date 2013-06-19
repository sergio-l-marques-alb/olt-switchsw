DEVDIR=/opt/eldk/usr/
PATH=$PATH:.

#ARCH=powerpc make distclean
ARCH=powerpc make menuconfig
ARCH=powerpc CROSS_COMPILE=$DEVDIR/bin/ppc_85xxDP- make
arch/powerpc/boot/dtc -O dtb -o cxp60g_matrix.dtb -b 0 -p 1024 arch/powerpc/boot/dts/cxp60g_uP2.dts
cp arch/powerpc/boot/uImage  ./kernelImageCXP60G_matrix.z
