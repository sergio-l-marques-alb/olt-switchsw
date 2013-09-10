#CROSS_COMPILE=/home/e-shadow/work/QorIQ_SDK-20110709-systembuilder/freescale-2010.09/bin/powerpc-linux-gnu-
CROSS_COMPILE=/opt/eldk/usr/bin/ppc_85xxDP-

CC='${CROSS-COMPILE}gcc'
CPPFLAGS='-I/$DEVDIR/include' 
AS='${CROSS_COMPILE}as'  
LD='${CROSS_COMPILE}ld' 
LIB='${CROSS_COMPILE}ar' 
RANLIB='$CROSS_COMPILE\ranlib' 
LLIBDIR='$DEVDIR/lib'  
NM='${CROSS_COMPILE}nm'

export CC CPPFLAGS AS LD LIB RANLIB LLIBDIR NM CROSS_COMPILE

make clean

ARCH=powerpc make menuconfig
ARCH=powerpc  make -Wall -j6 uImage


scripts/dtc/dtc -O dtb -o p1010_ptin_matriz.dtb -b 0 -p 1024 arch/powerpc/boot/dts/p1010_ptin_matriz.dts
cp arch/powerpc/boot/uImage  kernelImageP1010_matriz.z
