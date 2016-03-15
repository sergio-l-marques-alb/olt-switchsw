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
BOARD=$2

if [ $BOARD == "ta12xg" ]; then
 echo "Updating TA12XGE board...$1"
 cd output/FastPath-Ent-dpp-dnx-e500-LR-CSxw-IQH_TA12XG
 cp -v ipl/switchdrvr ipl/devshell_symbols.gz target/*.ko ipl/fp.cli ipl/fp.shell ipl/mgmd.cli $MKBOARDS/TA12XG/rootfs/usr/local/ptin/sbin/
 cp -v ipl/libmgmd.so $MKBOARDS/TA12XG/rootfs/usr/local/ptin/lib/
 cd - > /dev/null 2>&1
 cp -vr ../builds/tmp/TA12XG/rootfs/* $MKBOARDS/TA12XG/rootfs/
 echo "OK!"
fi

if [ $BOARD == "tu100g" ]; then
 echo "Updating TU100G board...$1"
 cd output/FastPath-Ent-dpp-dnx-e500-LR-CSxw-IQH_TU100G
 cp -v ipl/switchdrvr ipl/devshell_symbols.gz target/*.ko ipl/fp.cli ipl/fp.shell ipl/mgmd.cli $MKBOARDS/TU100G/rootfs/usr/local/ptin/sbin/
 cp -v ipl/libmgmd.so $MKBOARDS/TU100G/rootfs/usr/local/ptin/lib/
 cd - > /dev/null 2>&1
 cp -vr ../builds/tmp/TU100G/rootfs/* $MKBOARDS/TU100G/rootfs/
 echo "OK!"
fi

if [ $BOARD == "tt08sxg" ]; then
 echo "Updating TT08SXG board...$1"
 cd output/FastPath-Ent-dpp-dnx-e500mc-LR-CSxw-IQH_TT08SXG
 cp -v ipl/switchdrvr ipl/devshell_symbols.gz target/*.ko ipl/fp.cli ipl/fp.shell ipl/mgmd.cli $MKBOARDS/TT08SXG/rootfs/usr/local/ptin/sbin/
 cp -v ipl/libmgmd.so $MKBOARDS/TT08SXG/rootfs/usr/local/ptin/lib/
 cd - > /dev/null 2>&1
 cp -vr ../builds/tmp/TT08SXG/rootfs/* $MKBOARDS/TT08SXG/rootfs/
 echo "OK!"
fi

if [ $# -ge 3 ]; then
	IMAGE_VERSION=$3
	EQUIP_IP=$4
	echo -n "Generating image for version $3..."

        if [ $BOARD == "ta12xg" ]; then
                cd $MKBOARDS/TA12XG/
                sudo ./build_ramdisk_TA12XG.sh $IMAGE_VERSION > /dev/null 2>&1
                echo "OK!"
        fi

        if [ $BOARD == "tu100g" ]; then
                cd $MKBOARDS/TU100G/
                sudo ./build_ramdisk_TU100G.sh $IMAGE_VERSION > /dev/null 2>&1
                echo "OK!"
        fi

        if [ $BOARD == "tt08sxg" ]; then
                cd $MKBOARDS/TT08SXG/
                sudo ./build_ramdisk_TT08SXG.sh $IMAGE_VERSION > /dev/null 2>&1
                echo "OK!"
        fi

    if [ $# -ge 4 ]; then
	echo -n "Uploading image to $4..."
	./upload $EQUIP_IP
	cd - > /dev/null 2>&1
	echo "OK!"
    fi
fi

