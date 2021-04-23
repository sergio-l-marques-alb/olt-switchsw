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

if [ $# -eq 0 ]; then
 echo "Syntax: $0 <mkboards_folder> <board> <version> <IPaddr>"
 exit;
fi

elif [ $2 == "ag16ga" ]; then
 BOARD="AG16GA"
 MKBOARDS_FOLDER=AG16GA
 OUTPUT=output/FastPath-Ent-esw-xgs4-katanaarm-LR-CSxw-IQH_AG16GA
elif [ $2 == "ae48ge" ]; then
 BOARD="AE48GE"
 MKBOARDS_FOLDER=AE48GE
 OUTPUT=output/FastPath-Ent-esw-xgs4-armv7-LR-CSxw-IQH_AE48GE
else
 echo "Invalid board"
 exit;
fi

echo "Clearing output folders..."
mkdir -p $MKBOARDS/$MKBOARDS_FOLDER/rootfs/
mkdir -p $MKBOARDS/$MKBOARDS_FOLDER/rootfs/usr/local/ptin/sbin/
mkdir -p $MKBOARDS/$MKBOARDS_FOLDER/rootfs/usr/local/ptin/lib/
mkdir -p $MKBOARDS/$MKBOARDS_FOLDER/rootfs/usr/local/ptin/scripts/
mkdir $MKBOARDS/$MKBOARDS_FOLDER/backup/

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

#echo "Backing up unstripped binary..."
#cd $OUTPUT/ipl
#7z a $MKBOARDS/$MKBOARDS_FOLDER/backup/switchdrvr.unstripped_${BOARD}-${version}.7z switchdrvr.unstripped
#cd -
#echo "Done!"

if [ $# -ge 3 ]; then
	IMAGE_VERSION=$3
	EQUIP_IP=$4
	echo -n "Generating image for version $3..."

	cd $MKBOARDS/$MKBOARDS_FOLDER
	./build_ramdisk_${MKBOARDS_FOLDER}.sh $IMAGE_VERSION
        echo "OK!"

	if [ $# -ge 4 ]; then
		echo -n "Uploading image to $4..."
		./upload $EQUIP_IP
		cd - > /dev/null 2>&1
		echo "OK!"
	fi
fi

