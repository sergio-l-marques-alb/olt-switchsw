#!/bin/sh

#########################################################################################################
#                                                                                                       #
# Use this script to updated the desired mkboards folder for the requested card.                        #
# Optionally, you can also generate an image for the selected card and upload it to a remote equipment. #
#                                                                                                       #
# Usage:                                                                                                #
#               ./mkboards_update.sh <$MKBOARDS_PATH> <$CARD> [$VERSION $EQUIP_IP]                      #
#                                                                                                       #
# Note: This script requires permission elevation.                                                      #
#                                                                                                       #
#########################################################################################################

MKBOARDS=$1

if [ $# -eq 0 ]; then
 echo "Syntax: $0 <mkboards_folder> <board> <version> <IPaddr>"
 exit;
fi

BOARD=$2

if [ $2 == "cxo2t4" ]; then
 MKBOARDS_FOLDER=CXO2T4-MX
 OUTPUT=output/swdrv-dpp-dnx-cxo2t4-e500mc
elif [ $2 == "tt08sxg" ]; then
 MKBOARDS_FOLDER=TT08SXG
 OUTPUT=output/swdrv-dpp-dnx-tt08sxg-e500mc
elif [ $2 == "ta12xg" ]; then
 MKBOARDS_FOLDER=TA12XG
 OUTPUT=output/swdrv-dpp-dnx-ta12xg-e500
else
 echo "Invalid board"
 exit;
fi

echo "Clearing output folders..."
mkdir -p $MKBOARDS/$MKBOARDS_FOLDER/rootfs/
mkdir -p $MKBOARDS/$MKBOARDS_FOLDER/rootfs/usr/local/sbin/
mkdir -p $MKBOARDS/$MKBOARDS_FOLDER/rootfs/usr/local/lib/
mkdir -p $MKBOARDS/$MKBOARDS_FOLDER/rootfs/usr/local/scripts/
mkdir -p $MKBOARDS/$MKBOARDS_FOLDER/rootfs/usr/local/var/
mkdir $MKBOARDS/$MKBOARDS_FOLDER/backup/

echo "Updating $BOARD board..."
cd $OUTPUT
cp -v ipl/switchdrvr ipl/devshell_symbols.gz target/*.ko $MKBOARDS/$MKBOARDS_FOLDER/rootfs/usr/local/sbin/
#ipl/fp.cli ipl/fp.shell ipl/mgmd.cli 
cd - > /dev/null 2>&1
cp -vr ../builds/tmp/$BOARD/* $MKBOARDS/$MKBOARDS_FOLDER/rootfs/

version=r`svnversion .. -n | sed -e 's/.*://' -e 's/[A-Z]*$$//'`
echo echo Modular OLT $MODULE $version > ./fp.ver
echo echo "Built @ `date`" >> ./fp.ver
echo "echo OLTSWITCH md5sum: "`md5sum $OUTPUT/ipl/switchdrvr | awk '{print $1}'` >> ./fp.ver
chmod 777 ./fp.ver
cp ./fp.ver $MKBOARDS/$MKBOARDS_FOLDER/rootfs/usr/local/scripts/swdrvr-scripts
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

