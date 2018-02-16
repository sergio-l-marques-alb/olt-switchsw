CROSS_COMPILE=/opt/gcc-linaro-7.2.1-2017.11-i686_aarch64-linux-gnu/bin/aarch64-linux-gnu-
DEVDIR=/opt/gcc-linaro-7.2.1-2017.11-i686_aarch64-linux-gnu/

CC='${CROSS-COMPILE}gcc'
AS='${CROSS_COMPILE}as'
LD='${CROSS_COMPILE}ld'
AR='${CROSS_COMPILE}ar'
RANLIB='${CROSS_COMPILE}ranlib'
CXX='${CROSS_COMPILE}g++'
GDB='${CROSS_COMPILE}gdb'
NM='${CROSS_COMPILE}nm'
ARCH=arm64

export CROSS_COMPILE DEVDIR CC AS LD AR RANLIB CXX GDB NM ARCH

make clean
make distclean
make mrproper

cp  config-4.9.62 .config
make menuconfig 
cp .config config-4.9.62_refined

echo "Hello"
make -Wall -j`grep -c '^processor' /proc/cpuinfo`
echo "Done!"
