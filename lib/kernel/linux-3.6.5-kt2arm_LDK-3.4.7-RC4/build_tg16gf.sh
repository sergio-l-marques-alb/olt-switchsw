export ARCH=arm
#export CROSS_COMPILE=/opt/broadcom/bin/arm-linux-
export CROSS_COMPILE=/opt/broadcom_kt2_hx4/usr/bin/arm-linux-
export CHIP=katana2
export LOADADDR=0x61008000
#export LOADADDR=0x70000000

# Hide .svn folder
mv -v .svn .svn.tmp

make clean
make oldconfig 
make -j `grep -c '^processor' /proc/cpuinfo`
make modules -j `grep -c '^processor' /proc/cpuinfo`
make modules_install INSTALL_MOD_PATH=$PWD/rootfs
#instalar os include headers do linux
make headers_install ARCH=$ARCH INSTALL_HDR_PATH=$PWD/linux-inc

./mkimage -A arm -O linux -T kernel -n Image -a $LOADADDR -C none -d ./arch/arm/boot/Image ./tg16gf.kernel.z

# Restore .svn folder
mv -v .svn.tmp .svn
