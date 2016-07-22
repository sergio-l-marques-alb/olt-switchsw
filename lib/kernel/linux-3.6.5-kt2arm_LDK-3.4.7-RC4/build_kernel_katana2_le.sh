# Five Steps to make sure this kernel is self-compilable:
# 1. Copy bcmdrivers and bcmgpl drivers to kernel root path;
# 2. Modify bcmdrivers and bcmgpldrivers path in the following files:
#    -> arch/arm/plat-iproc/Makefile: obj-y+=../../../bcmdrivers/
#    -> drivers/mtd/Makefile: obj-y += (...) ../../bcmdrivers/(...)
# 3. Modify bcmdrivers and bcmgpldrivers path in the following files:
#    -> arch/arm/plat-iproc/Kconfig: source "bcmdrivers/Kconfig"
#    -> bcmdrivers/Kconfig: source "bcmdrivers/(...)" and source "bcmgpldrivers/(...)"
#    -> bcmgpldrivers/Kconfig: source "bcmgpldrivers/(...)"
# 4. Copy mkimage to kernel root path;
# 5. Modify uImage_flash.img destination within mk_uimage.sh script.

export ARCH=arm
#export CROSS_COMPILE=/opt/broadcom/bin/arm-linux-
export CROSS_COMPILE=/opt/broadcom_kt2_hx4/usr/bin/arm-linux-
export CHIP=katana2
export LOADADDR=0x61008000
#export LOADADDR=0x70000000
echo ""
echo "Exporting the sympbols..."
echo "export ARCH=$ARCH"
echo "export CROSS_COMPILE=$CROSS_COMPILE"
echo "export CHIP=$CHIP"
echo "export LOADADDR=$LOADADDR"
#cp ../../buildroot/board/broadcom/katana2/linux-3.6.5-be-initramfs.config .config #update config to stock config

# Hide .svn folder
mv -v .svn .svn.tmp

make clean
make menuconfig
make -j8

if [ $? -ne 0 ]; then
  echo ""
  echo "******* ERROR *******"
  echo "ERROR compiling linux kernel"
  echo ""
  exit 1
fi
cp ./arch/arm/boot/Image ./uImage_flash.img #Image is at arch/arm/boot/Image
echo "Using LOADADDR: $LOADADDR.";
echo "Image -> uImage"
sh mk_uimage.sh $LOADADDR # run mkimage

# Restore .svn folder
mv -v .svn.tmp .svn
