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

if [ $2 == "cxo640g" ]; then
 BOARD="CXO640G"
 MKBOARDS_FOLDER=CXO640G-MX
 OUTPUT=output/FastPath-Ent-esw-xgs4-pq3-LR-CSxw-IQH_CXO640G
elif [ $2 == "cxo160g" ]; then
 BOARD="CXO160G"
 MKBOARDS_FOLDER=CXO160G
 OUTPUT=output/FastPath-Ent-esw-xgs4-e500mc-LR-CSxw-IQH_CXO160G
elif [ $2 == "tg16g" ]; then
 BOARD="TG16G"
 MKBOARDS_FOLDER=TG16G
 OUTPUT=output/FastPath-Ent-esw-xgs4-pq2pro-LR-CSxw-IQH_TG16G
elif [ $2 == "ta48ge" ]; then
 BOARD="TA48GE"
 MKBOARDS_FOLDER=TA48GE
 OUTPUT=output/FastPath-Ent-esw-xgs4-e500-LR-CSxw-IQH_TA48GE
elif [ $2 == "olt1t0" ]; then
 BOARD="OLT1T0"
 MKBOARDS_FOLDER=CXOLT1T0
 OUTPUT=output/FastPath-Ent-esw-xgs4-helixarm-LR-CSxw-IQH_OLT1T0
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
	sudo ./build_ramdisk_${MKBOARDS_FOLDER}.sh $IMAGE_VERSION
        echo "OK!"

	if [ $# -ge 4 ]; then
		echo -n "Uploading image to $4..."
		./upload $EQUIP_IP
		cd - > /dev/null 2>&1
		echo "OK!"
	fi
fi

