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

SVN_NB_TOOLS_PATH=../../../../netband-tools
SVN_MGMD_TOOLS_PATH=../../../../netband-mgmd

# Check for folder existence
if [ ! -e "$SVN_NB_TOOLS_PATH" ]; then
	echo "Missing folders in $SVN_NB_TOOLS_PATH , please checkout NB_TOOLS from URL http://svn.ptin.corppt.com/repo/netband-tools/trunk"
	exit 1;
fi
if [ ! -e "$SVN_MGMD_TOOLS_PATH" ]; then
	echo "Missing folders in $SVN_MGMD_TOOLS_PATH , please checkout MGMD from URL http://svn.ptin.corppt.com/repo/netband-mgmd/trunk"
	exit 1;
fi


for plat in $PLATFORMS; do 
	sh ./nbtools_compile.sh $plat
	sh ./mgmd_compile.sh $plat
done
