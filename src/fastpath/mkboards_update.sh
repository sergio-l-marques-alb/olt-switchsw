#!/bin/sh

#########################################################################################################
#  																																	  #
# Use this script to updated the desired mkboards folder for the requested card. 							  #
# Optionally, you can also generate an image for the selected card and upload it to a remote equipment. #
#  																																	  #
# Usage: 																															  #
# 		./mkboards_update.sh <$MKBOARDS_PATH> <$CARD> [$VERSION $EQUIP_IP]  										  #
#  																																	  #
# Note: This script requires permission elevation. 																	  #
#  																																	  #
#########################################################################################################

MKBOARDS=$1

if [ $2 == "ta12xg" ]; then
 BOARD="TA12XG"
 OUTPUT=output/FastPath-Ent-dpp-dnx-e500-LR-CSxw-IQH_TA12XG
elif [ $2 == "tu100g" ]; then
 BOARD="TU100G"
 OUTPUT=output/FastPath-Ent-dpp-dnx-e500-LR-CSxw-IQH_TU100G
elif [ $2 == "tt08sxg" ]; then
 BOARD="TT08SXG"
 OUTPUT=output/FastPath-Ent-dpp-dnx-e500mc-LR-CSxw-IQH_TT08SXG
else
 echo "Invalid board"
 exit;
fi

echo "Clearing output folders..."
rm -rf $MKBOARDS/$BOARD/rootfs
mkdir -p $MKBOARDS/$BOARD/rootfs/
mkdir -p $MKBOARDS/$BOARD/rootfs/usr/local/ptin/sbin/
mkdir -p $MKBOARDS/$BOARD/rootfs/usr/local/ptin/lib/
mkdir -p $MKBOARDS/$BOARD/rootfs/usr/local/ptin/scripts/

echo "Updating $BOARD board..."
cd $OUTPUT
cp -v ipl/switchdrvr ipl/devshell_symbols.gz target/*.ko ipl/fp.cli ipl/fp.shell ipl/mgmd.cli $MKBOARDS/$BOARD/rootfs/usr/local/ptin/sbin/
cp -v ipl/libmgmd.so $MKBOARDS/$BOARD/rootfs/usr/local/ptin/lib/
cd - > /dev/null 2>&1
cp -vr ../builds/tmp/$BOARD/rootfs/* $MKBOARDS/$BOARD/rootfs/

version=r`svnversion .. -n | sed -e 's/.*://' -e 's/[A-Z]*$$//'`
echo echo Modular OLT $MODULE $version > ./fp.ver
echo echo "Built @ `date`" >> ./fp.ver
echo "echo OLTSWITCH md5sum: "`md5sum $OUTPUT/ipl/switchdrvr | awk '{print $1}'` >> ./fp.ver
chmod 777 ./fp.ver
cp ./fp.ver $MKBOARDS/$BOARD/rootfs/usr/local/ptin/sbin
echo "OK!"

echo "Backing up unstripped binary..."
cd $OUTPUT/ipl
7z a $MKBOARDS/$BOARD/backup/switchdrvr.unstripped_${BOARD}-${version}.7z switchdrvr.unstripped
cd -
echo "Done!"

if [ $# -ge 3 ]; then
	IMAGE_VERSION=$3
	EQUIP_IP=$4
	echo -n "Generating image for version $3..."

	cd $MKBOARDS/$BOARD/
	sudo ./build_ramdisk_${BOARD}.sh $IMAGE_VERSION > /dev/null 2>&1
        echo "OK!"

	if [ $# -ge 4 ]; then
		echo -n "Uploading image to $4..."
		./upload $EQUIP_IP
		cd - > /dev/null 2>&1
		echo "OK!"
	fi
fi

