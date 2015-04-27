#!/bin/bash

LDADDR=0x61008000;

echo "Setup Compiler Environment"

echo "source setup_arm_tools.bsh <dir>"
echo "/opt/broadcom  [full path required] "
#cd ../../ #iproc 
source ./setup_arm_tools.bsh /opt/broadcom
#cd - #current

#cp ../../buildroot/board/broadcom/helix4/linux-3.6.5-flash.config .config #update config to last config used
make clean
make menuconfig
make -j8 

if [ $? -ne 0 ]; then
  echo "ERROR compiling kernel"
  exit 1
fi

echo "Using LOADADDR: $LDADDR.";
echo "Image -> uImage"
./mk_uimage.sh $LDADDR

