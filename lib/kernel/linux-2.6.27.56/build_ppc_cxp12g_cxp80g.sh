DEVDIR=/opt/eldk/usr/
PATH=$PATH:.

#ARCH=powerpc make distclean
#cp config_cxp12G_uP2_2012Apr12 .config

ARCH=powerpc make menuconfig
ARCH=powerpc CROSS_COMPILE=$DEVDIR/bin/ppc_85xxDP- make

#scripts/dtc/dtc -O dtb -o cxp12g_uP2.dtb -b 0 -p 1024 arch/powerpc/boot/dts/cxp12g_uP2.dts
arch/powerpc/boot/dtc -O dtb -o cxp12g_matrix.dtb -b 0 -p 1024 arch/powerpc/boot/dts/cxp12g_uP2.dts

cp arch/powerpc/boot/uImage  ./kernelImageCXP12G_matrix.z

cp cxp12g_matrix.dtb cxp80g_matrix.dtb
cp kernelImageCXP12G_matrix.z kernelImageCXP80G_matrix.z

