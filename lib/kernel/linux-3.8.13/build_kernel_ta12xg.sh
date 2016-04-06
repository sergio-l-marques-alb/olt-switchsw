CROSS_COMPILE=/opt/eldk/usr/bin/ppc_85xxDP-
DEVDIR=/opt/eldk/usr

CC='${CROSS-COMPILE}gcc'
AS='${CROSS_COMPILE}as'  
LD='${CROSS_COMPILE}ld' 
AR='${CROSS_COMPILE}ar' 
RANLIB='${CROSS_COMPILE}ranlib'
CXX='${CROSS_COMPILE}g++'
GDB='${CROSS_COMPILE}gdb'  
NM='${CROSS_COMPILE}nm'
ARCH=powerpc

CONFIGURE_FLAGS="--target=powerpc-fsl-linux --host=powerpc-fsl-linux --build=i686-linux --with-libtool-sysroot=/opt/eldk"
CFLAGS=" -m32 -mhard-float  -mcpu=e500mc --sysroot=/opt/eldk"
CXXFLAGS=" -m32 -mhard-float  -mcpu=e500mc --sysroot=/opt/eldk"
LDFLAGS=" --sysroot=/opt/eldk"
CPPFLAGS=" -m32 -mhard-float  -mcpu=e500mc --sysroot=/opt/eldk"

#export CC AS LD CROSS_COMPILE ARCH RANLIB NM 
export CC AS CPPFLAGS LD  CROSS_COMPILE ARCH RANLIB NM CONFIGURE_FLAGS CFLAGS LDFLAGS CXXFLAGS

make clean
make menuconfig
make -Wall -j8 uImage


#scripts/dtc/dtc -O dtb -o cxo160g.dtb -b 0 -p 1024 arch/powerpc/boot/dts/p2041rdb.dts
cp arch/powerpc/boot/uImage ta12xg.z

