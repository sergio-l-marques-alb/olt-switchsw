#CROSS_COMPILE=/home/e-shadow/work/QorIQ_SDK-20110709-systembuilder/freescale-2010.09/bin/powerpc-linux-gnu-
CROSS_COMPILE=/opt/eldk/usr/bin/ppc_85xxDP-

CC='${CROSS_COMPILE}gcc'
CPPFLAGS='-I/$DEVDIR/include' 
AS='${CROSS_COMPILE}as'  
LD='${CROSS_COMPILE}ld' 
LIB='${CROSS_COMPILE}ar' 
RANLIB='$CROSS_COMPILE\ranlib' 
LLIBDIR='$DEVDIR/lib'  
NM='${CROSS_COMPILE}nm'

ARCH=powerpc

export CC CPPFLAGS AS LD LIB RANLIB LLIBDIR NM CROSS_COMPILE

if [ "$1" == "distclean" ]; then
  ARCH=powerpc make distclean mrproper
  ARCH=powerpc make ta48ge_defconfig
  ARCH=powerpc make clean
  make clean
  exit;
fi

mv -v .svn .svn.tmp

make clean
ARCH=powerpc make clean
#ARCH=powerpc make menuconfig
ARCH=powerpc make oldconfig
ARCH=powerpc make -Wall -l -j `grep -c '^processor' /proc/cpuinfo` uImage
ARCH=powerpc make -Wall -l -j `grep -c '^processor' /proc/cpuinfo` modules
ARCH=powerpc make -Wall -l -j `grep -c '^processor' /proc/cpuinfo` modules_install INSTALL_MOD_PATH=$PWD/rootfs
#ARCH=powerpc make -Wall -l -j `grep -c '^processor' /proc/cpuinfo` headers_install ARCH=$ARCH INSTALL_HDR_PATH=$PWD/linux-inc

scripts/dtc/dtc -O dtb -o ta48ge.dtb -b 0 -p 1024 arch/powerpc/boot/dts/p1014_ta48ge.dts
cp arch/powerpc/boot/uImage  ta48ge.kernel.z

mv -v .svn.tmp .svn
