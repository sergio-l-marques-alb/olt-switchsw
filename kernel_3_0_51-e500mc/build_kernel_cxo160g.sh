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

if [ $# -ge 1 ]; then
 if [ $1 = "distclean" ]; then
  make clean
  make distclean mrproper
  make cxo160g_defconfig
  exit;
 fi
fi

make clean
make menuconfig
make -Wall -j8 uImage

scripts/dtc/dtc -O dtb -o cxo160g.dtb -b 0 -p 1024 arch/powerpc/boot/dts/p2041rdb.dts
cp arch/powerpc/boot/uImage cxo160g.z

