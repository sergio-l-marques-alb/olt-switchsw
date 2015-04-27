export SDK=/home/mruas/repositorio/svn/helix4/sdk-xgs-robo-6.3.0

export SDK_NO_DRIVE_NAME=$SDK

export SDKBUILD=build

export TOOLCHAIN_BASE_DIR=/opt/broadcom
export TOOLCHAIN_BIN_DIR=$TOOLCHAIN_BASE_DIR/bin
export LD_LIBRARY_PATH=$TOOLCHAIN_BASE_DIR/lib

export COMPILER=$TOOLCHAIN_BIN_DIR/arm-linux-
export KERNEL_PATH=/home/olt/svnrepo/olt-switchsw/trunk/lib/kernel/linux-3.6.5-arm

export CROSS_COMPILE=$COMPILER
export KERNEL_SRC=$KERNEL_PATH

#teixeira: forçar a recompilação do socdiag
#/bin/rm -rf -f $SDK/build/unix-user/helixarm-2_6/socdiag.o

#cd $SDK/systems/linux/user/helixarm-2_6

gmake $1
