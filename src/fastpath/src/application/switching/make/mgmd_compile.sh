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

# 1 - Configuration
cd $MGMD_ROOT_DIR
if [ -f "configure" ]; then
	#Compare MGMD package version and current configured version
	MGMD_PACKAGE_VERSION=`cat configure.ac | grep "AC_INIT" -m 1 | sed "s/[^0-9]*//; s/].*//"`
	MGMD_CURRENT_VERSION=`cat configure | grep "PACKAGE_VERSION=" -m 1 | sed "s/[^0-9]*//; s/'//"`
	if [ "$MGMD_PACKAGE_VERSION" != "$MGMD_CURRENT_VERSION" ]; then
		echo "MGMD upgrade detected! Reconfiguring..."
		sh build/autogen.sh && sh build/configure-ud.sh
	fi
else
	#MGMD not yet configured. To reduce Fastpath's compilation verbosity, we skip the configuration output
	echo "Starting MGMD configuration..."
	sh build/autogen.sh >/dev/null 2>&1 && sh build/configure-ud.sh >/dev/null 2>&1
fi

# 2 - Compilation
if [ "$SWITCHING_COMPILE_TARGET" = "switching" ]; then
	make -j1 all && make install >/dev/null 2>&1
	cp rfs/usr/local/ptin/include/mgmd/* $FASTPATH_PUBLIC_API_DIR
elif [ "$SWITCHING_COMPILE_TARGET" = "clean-switching" ]; then
	make clean distclean >/dev/null 2>&1
fi
cd -

