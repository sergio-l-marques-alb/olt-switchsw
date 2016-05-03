#!/bin/sh

#########################################################################################################
#  													#
# Use this script to updated the desired mkboards folder for the requested card. 			#
# Optionally, you can also generate an image for the selected card and upload it to a remote equipment. #
#  													#
# Usage: 												#
# 		./mkboards_update.sh <$MKBOARDS_PATH> <$CARD> [$VERSION $EQUIP_IP]  			#
#  													#
# Note: This script requires permission elevation. 							#
#  													#
#########################################################################################################

if [ $# -eq 0 ]; then
 echo "Syntax: $0 <mkboards_path> <board> [version] [IPaddr]"
 exit;
fi

MKBOARDS=$1

if [ $2 == "ta12xg" ]; then
 BOARD="TA12XG"
 MKBOARDS_FOLDER=$BOARD
 OUTPUT=output/FastPath-Ent-dpp-dnx-e500-LR-CSxw-IQH_TA12XG
elif [ $2 == "tu100g" ]; then
 BOARD="TU100G"
 MKBOARDS_FOLDER=$BOARD
 OUTPUT=output/FastPath-Ent-dpp-dnx-e500-LR-CSxw-IQH_TU100G
elif [ $2 == "tt08sxg" ]; then
 BOARD="TT08SXG"
 MKBOARDS_FOLDER=$BOARD
 OUTPUT=output/FastPath-Ent-dpp-dnx-e500mc-LR-CSxw-IQH_TT08SXG
else
 echo "Invalid board"
 exit;
fi

echo "Clearing output folders..."
mkdir -p $MKBOARDS/$MKBOARDS_FOLDER/rootfs/
mkdir -p $MKBOARDS/$MKBOARDS_FOLDER/rootfs/usr/local/ptin/sbin/
mkdir -p $MKBOARDS/$MKBOARDS_FOLDER/rootfs/usr/local/ptin/lib/
mkdir -p $MKBOARDS/$MKBOARDS_FOLDER/rootfs/usr/local/ptin/scripts/
mkdir -p $MKBOARDS/$MKBOARDS_FOLDER/backup/

echo "Updating $BOARD board..."
cd $OUTPUT
cp -v ipl/switchdrvr ipl/devshell_symbols.gz target/*.ko ipl/fp.cli ipl/fp.shell ipl/mgmd.cli $MKBOARDS/$MKBOARDS_FOLDER/rootfs/usr/local/ptin/sbin/
cp -v ipl/libmgmd.so $MKBOARDS/$MKBOARDS_FOLDER/rootfs/usr/local/ptin/lib/
cd - > /dev/null 2>&1
cp -vr ../builds/tmp/$MKBOARDS_FOLDER/rootfs/* $MKBOARDS/$MKBOARDS_FOLDER/rootfs/

version=r`svnversion .. -n | sed -e 's/.*://' -e 's/[A-Z]*$$//'`
echo echo Modular OLT $MODULE $version > ./fp.ver
echo echo "Built @ `date`" >> ./fp.ver
echo "echo OLTSWITCH md5sum: "`md5sum $OUTPUT/ipl/switchdrvr | awk '{print $1}'` >> ./fp.ver
chmod 777 ./fp.ver
cp ./fp.ver $MKBOARDS/$MKBOARDS_FOLDER/rootfs/usr/local/ptin/sbin
echo "OK!"

echo "Backing up unstripped binary..."
cd $OUTPUT/ipl
7z a $MKBOARDS/$MKBOARDS_FOLDER/backup/switchdrvr.unstripped_${BOARD}-${version}.7z switchdrvr.unstripped
cd -
echo "Done!"

if [ $# -ge 3 ]; then
	IMAGE_VERSION=$3
	EQUIP_IP=$4
	echo -n "Generating image for version $3..."

	cd $MKBOARDS/$MKBOARDS_FOLDER/
	sudo ./build_ramdisk_${MKBOARDS_FOLDER}.sh $IMAGE_VERSION

	if [ $# -ge 4 ]; then
		echo -n "Uploading image to $4..."
		./upload $EQUIP_IP
		cd - > /dev/null 2>&1
		echo "OK!"
	fi
fi

