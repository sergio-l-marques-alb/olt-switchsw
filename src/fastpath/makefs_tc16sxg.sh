#!/bin/bash

BOARD=TC16SXG
DST_PATH=output/FastPath-Ent-esw-xgs4-td3x3arm-LR-CSxw-IQH_TC16SXG
TGZ_FILE=switchdrvr_rootfs.tgz

CURR_PATH=`pwd`
# Absolute path to this script
SCRIPT=`readlink -f $0`
# Absolute path of this script
SCRIPT_PATH=`dirname $SCRIPT`

echo "Script syntax: $0 [rootfs_dst_path] [IP_target_equipment]"

# Change to dir where script is
echo "Current path is $CURR_PATH"
echo "Changing to $SCRIPT_PATH..."
cd $SCRIPT_PATH

# Remove any rootfs made before
echo "Cleaning old rootfs..."
rm -rf $DST_PATH/rootfs

# Copy base rootfs to destination
echo "Copying base rootfs..."
cp -rv ../builds/tmp/TC16SXG/rootfs $DST_PATH

# Change to board output dir
cd $DST_PATH

# Update rootfs with compiled binaries
echo "Adding binaries..."
cp ipl/switchdrvr ipl/devshell_symbols.gz ipl/fp.cli ipl/fp.shell target/*.ko rootfs/usr/local/ptin/sbin/

# Make tgz
echo "Building tarball..."
cd rootfs
tar czvf ../$TGZ_FILE *
cd ..

echo "Built tarball located at $SCRIPT_PATH/$DST_PATH/$TGZ_FILE"

if [ $# -ge 1 ]; then
  echo "Decompressing $TGZ_FILE to $1..."
  tar xzvf $TGZ_FILE -C $1/rootfs
  cd $1
  ./build/build_ramdisk_TC16SXG.sh

  if [ $# -ge 2 ]; then
    lc_put $2 /usr/local/ptin/boot_images/tolts rdimg_TC16SXG.gz
  fi
fi

# Return to original path
cd $CURR_PATH
echo "Done!"
