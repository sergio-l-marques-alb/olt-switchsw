#!/bin/bash

LDADDR=0x61008000;
export ARCH=arm
export CROSS_COMPILE=/opt/broadcom/bin/arm-linux-
export CHIP=helix4
export LOADADDR=0x61008000
echo ""
echo "Exporting the sympbols..."
echo "export ARCH=$ARCH"
echo "export CROSS_COMPILE=$CROSS_COMPILE"
echo "export CHIP=$CHIP"
echo "export LOADADDR=$LOADADDR"

echo "Setup Compiler Environment"

#echo "source setup_arm_tools.bsh <dir>"
#echo "/opt/broadcom  [full path required] "
#cd ../../ #iproc 
#source setup_arm_tools.bsh /opt/broadcom
#cd kernel/linux-3.6.5/ #current

#cp ../../buildroot/board/broadcom/helix4/linux-3.6.5-flash.config .config #update config to last config used

make clean
make menuconfig
make -j8 
make modules

if [ $? -ne 0 ]; then
  echo "ERROR compiling kernel"
  exit 1
fi

echo "Using LOADADDR: $LDADDR.";
echo "Image -> uImage"
../../buildroot/mk_uimage_ptin.sh $LDADDR



