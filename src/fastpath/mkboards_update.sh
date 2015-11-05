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
if [ $# -lt 2 ]; then
echo "./mkboards_update.sh <\$MKBOARDS_PATH> <\$CARD> [\$VERSION \$EQUIP_IP]"
exit -1
fi

MKBOARDS=$1
BOARD=$2

if [ $BOARD == "tg16g" ]; then
 echo -n "Updating TG16G mkboard...$1"
 cd output/FastPath-Ent-esw-xgs4-pq2pro-LR-CSxw-IQH_TG16G
 cp -v ipl/switchdrvr ipl/devshell_symbols.gz target/*.ko ipl/fp.cli ipl/fp.shell ipl/mgmd.cli $MKBOARDS/TG16G/rootfs/usr/local/ptin/sbin/
 cp -v ipl/libmgmd.so $MKBOARDS/TG16G/rootfs/usr/local/ptin/lib/
 cd - > /dev/null 2>&1
 echo "OK!"
fi

if [ $BOARD == "cxo640g" ]; then
 echo -n "Updating CXO640G board...$1"
 cd output/FastPath-Ent-esw-xgs4-pq3-LR-CSxw-IQH_CXO640G
 cp -v ipl/switchdrvr ipl/devshell_symbols.gz target/*.ko ipl/fp.cli ipl/fp.shell ipl/mgmd.cli $MKBOARDS/CXO640G-MX/rootfs/usr/local/ptin/sbin/
 cp -v ipl/libmgmd.so $MKBOARDS/CXO640G-MX/rootfs/usr/local/ptin/lib/
 cd - > /dev/null 2>&1
 echo "OK!"
fi

if [ $BOARD == "cxo160g" ]; then
 echo -n "Updating CXO160G board...$1"
 cd output/FastPath-Ent-esw-xgs4-e500mc-LR-CSxw-IQH_CXO160G
 cp -v ipl/switchdrvr ipl/devshell_symbols.gz target/*.ko ipl/fp.cli ipl/fp.shell ipl/mgmd.cli $MKBOARDS/CXO160G/rootfs/usr/local/ptin/sbin/
 cp -v ipl/libmgmd.so $MKBOARDS/CXO160G/rootfs/usr/local/ptin/lib/
 cd - > /dev/null 2>&1
 echo "OK!"
fi

if [ $BOARD == "ta48ge" ]; then
 echo -n "Updating TA48GE board...$1"
 cd output/FastPath-Ent-esw-xgs4-e500-LR-CSxw-IQH_TA48GE
 cp -v ipl/switchdrvr ipl/devshell_symbols.gz target/*.ko ipl/fp.cli ipl/fp.shell ipl/mgmd.cli $MKBOARDS/TA48GE/rootfs/usr/local/ptin/sbin/
 cp -v ipl/libmgmd.so $MKBOARDS/TA48GE/rootfs/usr/local/ptin/lib/
 cd - > /dev/null 2>&1
 echo "OK!"
fi

if [ $# -ge 3 ]; then
	IMAGE_VERSION=$3
	EQUIP_IP=$4	
	echo -n "Generating image for version $3..."

	if [ $BOARD == "tg16g" ]; then	
		cd $MKBOARDS/TG16G/
		sudo ./build_ramdisk_TG16G.sh $IMAGE_VERSION > /dev/null 2>&1
		if  [ $? -ne 0 ]; then
			echo "Failed to generate ramdisk!"
			exit $?
		fi
		echo "OK!"
	fi

	if [ $BOARD == "cxo160g" ]; then
		cd $MKBOARDS/CXO160G/
		sudo ./build_ramdisk_CXO160G.sh $IMAGE_VERSION > /dev/null 2>&1
		if  [ $? -ne 0 ]; then
                        echo "Failed to generate ramdisk!"
                        exit $?
                fi
		echo "OK!"
	fi

	if [ $BOARD == "cxo640g" ]; then
		cd $MKBOARDS/CXO640G-MX/
		sudo ./build_ramdisk_CXO640G-MX.sh $IMAGE_VERSION > /dev/null 2>&1
		if  [ $? -ne 0 ]; then
                        echo "Failed to generate ramdisk!"
                        exit $?
                fi
		echo "OK!"
	fi

	if [ $BOARD == "ta48ge" ]; then
		cd $MKBOARDS/TA48GE/
		sudo ./build_ramdisk_TA48GE.sh $IMAGE_VERSION > /dev/null 2>&1
		if  [ $? -ne 0 ]; then
                        echo "Failed to generate ramdisk!"
                        exit $?
                fi
		echo "OK!"
	fi

    if [ $# -ge 4 ]; then
	echo -n "Uploading image to $4..."
	./upload $EQUIP_IP
	cd - > /dev/null 2>&1
	echo "OK!"
    fi
fi

