##############################################
#                                            #
# MGMD configure script for the TA48GE card  #
#                                            #
#	Daniel Figueira, 2014                     #
#	daniel-f-figueira@ext.ptinovacao.pt       #
#                                            #
##############################################
#!/bin/sh

# README!
#
# This is the script which configures MGMD for this specific card.
# First, it starts by replacing the values in mgmd_configure.template with the ones for this card.
# Then, it proceeds to compare MGMD configure.sh and mgmd_configure.template md5sum:
#		- If they match, then MGMD was compiled for this card the last time. No need to reconfigure and recompile;
#		- If they differ, we need to replace MGMD configure file and force a new compilation.
#
# If you're reading this file because you need to support a new card, then these are the only steps that you MUST follow:
#		- Create a new mgmd_config_$(CARD).sh file in the root directory of Fastpath, copied from an existing configure file for another card;
#			- This step may be tricky...You need to ensure that the $(CARD) part of the name of the script is the same as the 'CARD' value outputted in the card 'welcome' Makefile's target!
#		- Change the values defined in '2 - Card variables' for the newly created script. Do not change anything else!


# 1 - Paths and files
MGMD_PATH=src/application/switching/mgmd
MGMD_CONFIG_FILE=$MGMD_PATH/build/configure-mgmd.sh
TEMPLATE_CONFIG_FILE=$MGMD_PATH/build/configure-ud.sh
TEMPORARY_CONFIG_FILE=$MGMD_PATH/build/configure-mgmd.sh.temp

# 2 - Card variables
CARD=TT08SXG
COMPILER_DIR=/opt/fsl/1.2/sysroots/i686-fslsdk-linux/usr/bin/ppce500mc-fsl-linux
COMPILER_PREFIX=powerpc-fsl-linux-
MAX_CHANNELS=4096
MAX_WHITELIST=16384
MAX_CLIENTS=128
MAX_FP_PORTS=8
MAX_FP_PORT_ID=12
MAX_SERVICES=40
MAX_SERVICE_ID=255
ADMISSION_CONTROL_SUPPORT=1
ROOT_PORT_IS_ON_MAX_PORT_ID=1

# 3 - Modify template file with TG16G values. DO NOT CHANGE THIS!
cp $TEMPLATE_CONFIG_FILE $TEMPORARY_CONFIG_FILE
sed -i "s|export COMPILER_DIR=.*|export COMPILER_DIR=$COMPILER_DIR|" $TEMPORARY_CONFIG_FILE
sed -i "s|export COMPILER_PREFIX=.*|export COMPILER_PREFIX=$COMPILER_PREFIX|" $TEMPORARY_CONFIG_FILE
sed -i "s|.*export PTIN_MGMD_PLATFORM_MAX_CHANNELS=.*|   export PTIN_MGMD_PLATFORM_MAX_CHANNELS=$MAX_CHANNELS|" $TEMPORARY_CONFIG_FILE
sed -i "s|.*export PTIN_MGMD_PLATFORM_MAX_WHITELIST=.*|   export PTIN_MGMD_PLATFORM_MAX_WHITELIST=$MAX_WHITELIST|" $TEMPORARY_CONFIG_FILE
sed -i "s|.*export PTIN_MGMD_PLATFORM_MAX_CLIENTS=.*|   export PTIN_MGMD_PLATFORM_MAX_CLIENTS=$MAX_CLIENTS|" $TEMPORARY_CONFIG_FILE
sed -i "s|.*export PTIN_MGMD_PLATFORM_MAX_PORTS=.*|   export PTIN_MGMD_PLATFORM_MAX_PORTS=$MAX_FP_PORTS|" $TEMPORARY_CONFIG_FILE
sed -i "s|.*export PTIN_MGMD_PLATFORM_MAX_PORT_ID=.*|   export PTIN_MGMD_PLATFORM_MAX_PORT_ID=$MAX_FP_PORT_ID|" $TEMPORARY_CONFIG_FILE
sed -i "s|.*export PTIN_MGMD_PLATFORM_MAX_SERVICES=.*|   export PTIN_MGMD_PLATFORM_MAX_SERVICES=$MAX_SERVICES|" $TEMPORARY_CONFIG_FILE
sed -i "s|.*export PTIN_MGMD_PLATFORM_MAX_SERVICE_ID=.*|   export PTIN_MGMD_PLATFORM_MAX_SERVICE_ID=$MAX_SERVICE_ID|" $TEMPORARY_CONFIG_FILE
sed -i "s|.*export PTIN_MGMD_PLATFORM_ADMISSION_CONTROL_SUPPORT=.*|   export PTIN_MGMD_PLATFORM_ADMISSION_CONTROL_SUPPORT=$ADMISSION_CONTROL_SUPPORT|" $TEMPORARY_CONFIG_FILE
sed -i "s|.*export PTIN_MGMD_PLATFORM_ROOT_PORT_IS_ON_MAX_PORT_ID=.*|   export PTIN_MGMD_PLATFORM_ROOT_PORT_IS_ON_MAX_PORT_ID=$ROOT_PORT_IS_ON_MAX_PORT_ID|" $TEMPORARY_CONFIG_FILE

# 4 - Check config files md5sum
TEMPORARY_CONFIG_FILE_MD5SUM=`md5sum $TEMPORARY_CONFIG_FILE | awk '{ print $1 }'`
MGMD_CONFIG_FILE_MD5SUM=
if [ -f "$MGMD_CONFIG_FILE" ]; then
	MGMD_CONFIG_FILE_MD5SUM=`md5sum $MGMD_CONFIG_FILE | awk '{ print $1 }'`
fi

# 5 - If checksums match, the last MGMD compilation was for this card. Otherwise, we should remove configure file and force a new compilation
if [ "$TEMPORARY_CONFIG_FILE_MD5SUM" != "$MGMD_CONFIG_FILE_MD5SUM" ]; then
	echo "Configuring MGMD for $CARD card"
	make -C $MGMD_PATH clean distclean >/dev/null 2>&1
	cp $TEMPORARY_CONFIG_FILE $MGMD_CONFIG_FILE
	rm -f $MGMD_PATH/configure
fi
rm $TEMPORARY_CONFIG_FILE


