#!/bin/sh

# README!
#
# This script configures and compiles MGMD and NB_TOOLS modules and moves the 
# resulting .h and .so to a build_dir folder
#
#
# Author:  Rui Fernandes (rui-f-fernandes@alticelabs.com)
# Date:    2018-02-19


PLATFORMS="cxo2t4 ta12xg tt08sxg"

export PACKAGES_DIR=$PWD/../../../..
export NB_TOOLS_PATH=$PACKAGES_DIR/netband-tools/trunk
export MGMD_TOOLS_PATH=$PACKAGES_DIR/netband-mgmd/trunk

# Check for folder existence
if [ ! -e "$NB_TOOLS_PATH" ]; then
	echo "Missing folders in $NB_TOOLS_PATH , please checkout NB_TOOLS from URL http://svn.ptin.corppt.com/repo/netband-tools/trunk"
	exit 1;
fi
if [ ! -e "$MGMD_TOOLS_PATH" ]; then
	echo "Missing folders in $MGMD_TOOLS_PATH , please checkout MGMD from URL http://svn.ptin.corppt.com/repo/netband-mgmd/trunk"
	exit 1;
fi

echo "Cleaning and Compiling NBTOOLS"
for plat in $PLATFORMS; do 
	sh ./nbtools_compile.sh $plat clean
	if [ $# -ne 0 ]; then
		echo "[NBTOOLS] Error cleaning for $plat"
		exit 1
	fi
	sh ./nbtools_compile.sh $plat
        if [ $# -ne 0 ]; then
                echo "[NBTOOLS] Error compiling for $plat"
                exit 1
        fi
done

echo "Cleaning and Compiling MGMD"
for plat in $PLATFORMS; do
        sh ./mgmd_compile.sh $plat clean
        if [ $# -ne 0 ]; then
                echo "[MGMD] Error cleaning for $plat"
                exit 1
        fi
        sh ./mgmd_compile.sh $plat
	if [ $# -ne 0 ]; then
                echo "[MGMD] Error compiling for $plat"
                exit 1
        fi
done
