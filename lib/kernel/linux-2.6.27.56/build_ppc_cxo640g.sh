DEVDIR=/opt/eldk/usr/

export PATH=./:$PATH

mv -v .svn .svn.tmp

cp config_cxo360G_uP2 .config

#ARCH=ppc make distclean
#ARCH=powerpc make menuconfig
ARCH=powerpc CROSS_COMPILE=$DEVDIR/bin/ppc_85xxDP- make -j `grep -c '^processor' /proc/cpuinfo`
arch/powerpc/boot/dtc -O dtb -o cxp360g_matrix.dtb -b 0 -p 1024 arch/powerpc/boot/dts/cxp360g_uP2.dts
cp arch/powerpc/boot/uImage  ./kernelImageCXP360G_matrix.z 

mv -v .svn.tmp .svn

