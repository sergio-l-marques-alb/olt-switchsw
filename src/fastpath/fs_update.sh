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

BOARD=$1

if [ $BOARD == "TA12XG" ]; then
 OUTPUT=output/FastPath-Ent-dpp-dnx-e500-LR-CSxw-IQH_TA12XG
elif [ $BOARD == "TU100G" ]; then
 OUTPUT=output/FastPath-Ent-dpp-dnx-e500-LR-CSxw-IQH_TU100G
elif [ $BOARD == "TT08SXG" ]; then
 OUTPUT=output/FastPath-Ent-dpp-dnx-e500mc-LR-CSxw-IQH_TT08SXG
else
 echo "Unknown board!"
 exit 0;
fi

rm -rf tmp/$BOARD
mkdir -p tmp/$BOARD
cp -vr ../builds/tmp/$BOARD/rootfs tmp/$BOARD

mkdir -p tmp/$BOARD/rootfs/usr/local/ptin/sbin/
cp -v $OUTPUT/ipl/switchdrvr $OUTPUT/ipl/devshell_symbols.gz $OUTPUT/target/*.ko $OUTPUT/ipl/fp.cli $OUTPUT/ipl/fp.shell $OUTPUT/ipl/mgmd.cli tmp/$BOARD/rootfs/usr/local/ptin/sbin/
mkdir -p tmp/$BOARD/rootfs/usr/local/ptin/lib/
cp -v $OUTPUT/ipl/libmgmd.so tmp/$BOARD/rootfs/usr/local/ptin/lib/

echo "Compressing filesystem..."
cd tmp
tar czvf $BOARD-fs.tgz $BOARD
cd -

echo "Done!"
