ARCH=powerpc CROSS_COMPILE=/opt/eldk/usr/bin/ppc_85xxDP- make menuconfig
ARCH=powerpc CROSS_COMPILE=/opt/eldk/usr/bin/ppc_85xxDP- make -j4

cp arch/powerpc/boot/uImage ../ta12xg.z
