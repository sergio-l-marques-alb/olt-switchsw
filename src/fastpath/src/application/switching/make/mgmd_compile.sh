#########################################
#                                       #
# Script to configure and compile MGMD  #
#                                       #
#  Daniel Figueira, 2014                #
#  daniel-f-figueira@ext.ptinovacao.pt  #
#                                       #
#########################################
#!/bin/sh
 
# README!
#
# This script is invoked by 'makerules.targets' in src/l7tools directory.
#
# Input variables:
#		- $1 -> Absolute path for the Fastpath's root directory
#		- $2 -> Target with which the switching component is to be compiled ('switching'; 'clean-switching') 
#
# This script is responsible for MGMD configuration and compilation.
# To optimize the compilation process, this script will only reconfigure MGMD in two situations:
#		- If the configure file does not exist, then MGMD is not configured and hence, we need to force the configuration;
#		- If the configure file exists, but the version reported differs from the one in configure.ac, then MGMD was updated and we need to force a re-configuration.
# The make target with which MGMD is compiled is determined based on the 'switching' target.


FASTPATH_ROOT_DIR=$1
SWITCHING_COMPILE_TARGET=$2

MGMD_ROOT_DIR=$FASTPATH_ROOT_DIR/src/application/switching/mgmd/
FASTPATH_PUBLIC_API_DIR=$FASTPATH_ROOT_DIR/src/l7public/api

NEW_CONFIGURATION="no"


# 1 - Configuration
cd $MGMD_ROOT_DIR
	
if [ -f "configure" ]; then
	#Compare MGMD package version and current configured version
	MGMD_PACKAGE_VERSION=`cat ./src/ptin_mgmd_defs.h | grep "PTIN_MGMD_SVN_PACKAGE" -m 1|  awk '{print$3}'  | sed "s/[^0-9.M]*//g; s/].*//"`	
	#echo "MGMD_PACKAGE_VERSION:"$MGMD_PACKAGE_VERSION
	MGMD_CURRENT_VERSION=`cat configure | grep "PACKAGE_VERSION=" -m 1 | sed "s/[^0-9]*//; s/'//"`
	#echo "MGMD_CURRENT_VERSION:"$MGMD_CURRENT_VERSION
	if [ "$MGMD_PACKAGE_VERSION" != "$MGMD_CURRENT_VERSION" ]; then
		echo "MGMD upgrade detected! Reconfiguring..."
		NEW_CONFIGURATION="yes"
		rm configure
		sh build/autogen.sh && sh build/configure-mgmd.sh >/dev/null 2>&1
	fi
else
	#MGMD not yet configured. To reduce Fastpath's compilation verbosity, we skip the configuration output
	echo "Starting MGMD configuration..."
	NEW_CONFIGURATION="yes"
	sh build/autogen.sh >/dev/null 2>&1 && sh build/configure-mgmd.sh >/dev/null 2>&1
fi

# 2 - Compilation
if [ "$SWITCHING_COMPILE_TARGET" = "switching" ]; then
	make -j1 all && make install >/dev/null 2>&1
elif [ "$SWITCHING_COMPILE_TARGET" = "clean-switching" ]; then
	make clean distclean >/dev/null 2>&1
fi

if [ "$NEW_CONFIGURATION" = "yes" ]; then
	cp rfs/usr/local/ptin/include/mgmd/* $FASTPATH_PUBLIC_API_DIR
fi
cd -
