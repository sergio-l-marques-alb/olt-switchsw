NAME=tg16g

export PATH=$PATH:./uboot-tools

DEVDIR=/opt/freescale/usr/local/gcc-4.0.2-glibc-2.3.6-nptl-2/powerpc-e300c3-linux
CROSS=$DEVDIR/bin/powerpc-e300c3-linux-

UBOOTDIR="./uboot-tools"


if [ "$1" = "dts" ];then

scripts/dtc/dtc -O dtb ${NAME}.dts -o ${NAME}.dtb
#cp ${NAME}.dtb /tftpboot
#sudo cp ${NAME}.dtb /tftpboot

elif [ "$1" = "clean" ];then

ARCH=powerpc make clean

elif [ "$1" = "distclean" ];then

ARCH=powerpc make clean distclean mrproper
ARCH=powerpc make tg16g_defconfig #mpc834x_mds_defconfig #mpc8313erdb_defconfig

elif [ "$1" = "menuconfig" ];then

ARCH=powerpc make menuconfig 

else

mv -v .svn .svn.tmp

rm vmlinux vmlinux2.gz kernelImage.z
ARCH=powerpc make oldconfig
#ARCH=powerpc CROSS_COMPILE=${CROSS} make -j4 vmlinux
ARCH=powerpc CROSS_COMPILE=${CROSS} make -j `grep -c '^processor' /proc/cpuinfo`
cp arch/powerpc/boot/uImage ${NAME}.kernel.z

if [ "$1" = "all" ];then

${CROSS}objcopy --strip-all -S -O binary vmlinux vmlinux2
gzip -vf9 vmlinux2
${UBOOTDIR}/mkimage -n 'linux-2.6.38.8-denx-ptin' -A ppc -O linux -T kernel -C gzip -a 00000000 -e 00000000 -d vmlinux2.gz ${NAME}.kernel.z
rm vmlinux2.gz
#cp ${NAME}.kernel.z /tftpboot/
#sudo cp ${NAME}.kernel.z /tftpboot/

fi

mv -v .svn.tmp .svn

fi

