#!/bin/sh

# README!
#
# This script configures and compiles MGMD module integrated with Fastpath
#
# Input variables:
#		- $1 -> Card name
#
# Author:  Alexandre R. Santos (alexandre-r-santos@alticelabs.com)
# Date:    2017-06-01

NUM_CPUS=`grep -c 'model name' /proc/cpuinfo`

# In case card is not specified as a parameter, check if environment variables are set
if [ $# -eq 0 ]; then
  echo "[MGMD] Please use $0 <card name>!"
  exit 1
fi

echo "[MGMD] Configuring $1 card"

# Define specific variables according to the selected card
if [ "$1" == "TG16GF" ]; then
  BOARD=$1
  CPU=katanaarm
  
  export COMPILER_DIR=/opt/broadcom_kt2_hx4/usr/bin
  export COMPILER_PREFIX=arm-linux-
  export LD_LIB_PATH=/opt/broadcom_kt2_hx4/usr/lib

  # Overide local variables with the ones comming from the makefile (if defined)
  export COMPILER_DIR="${TOOLCHAIN_BIN_DIR:-$COMPILER_DIR}"
  PREFIX=`echo $COMPILER | awk -F'/' '{print $NF}'`
  if [ ! -z $PREFIX ]; then export COMPILER_PREFIX=$PREFIX; fi
  export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-$LD_LIB_PATH}"
  
  export PTIN_MGMD_PLATFORM_MAX_CHANNELS=4096
  export PTIN_MGMD_PLATFORM_MAX_WHITELIST=16384
  export PTIN_MGMD_PLATFORM_MAX_CLIENTS=512
  export PTIN_MGMD_PLATFORM_MAX_PORTS=16
  export PTIN_MGMD_PLATFORM_MAX_PORT_ID=23
  export PTIN_MGMD_PLATFORM_MAX_SERVICES=40
  export PTIN_MGMD_PLATFORM_MAX_SERVICE_ID=255
  export PTIN_MGMD_PLATFORM_ADMISSION_CONTROL_SUPPORT=1
  export PTIN_MGMD_PLATFORM_ROOT_PORT_IS_ON_MAX_PORT_ID=1

elif [ "$1" == "AG16GA" ]; then
  BOARD=$1
  CPU=katanaarm

  export COMPILER_DIR=/opt/broadcom_kt2_hx4/usr/bin
  export COMPILER_PREFIX=arm-linux-
  export LD_LIB_PATH=/opt/broadcom_kt2_hx4/usr/lib

  # Overide local variables with the ones comming from the makefile (if defined)
  export COMPILER_DIR="${TOOLCHAIN_BIN_DIR:-$COMPILER_DIR}"
  PREFIX=`echo $COMPILER | awk -F'/' '{print $NF}'`
  if [ ! -z $PREFIX ]; then export COMPILER_PREFIX=$PREFIX; fi
  export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-$LD_LIB_PATH}"

  export PTIN_MGMD_PLATFORM_MAX_CHANNELS=4096
  export PTIN_MGMD_PLATFORM_MAX_WHITELIST=16384
  export PTIN_MGMD_PLATFORM_MAX_CLIENTS=512
  export PTIN_MGMD_PLATFORM_MAX_PORTS=16
  export PTIN_MGMD_PLATFORM_MAX_PORT_ID=23
  export PTIN_MGMD_PLATFORM_MAX_SERVICES=40
  export PTIN_MGMD_PLATFORM_MAX_SERVICE_ID=255
  export PTIN_MGMD_PLATFORM_ADMISSION_CONTROL_SUPPORT=1
  export PTIN_MGMD_PLATFORM_ROOT_PORT_IS_ON_MAX_PORT_ID=1

elif [ "$1" == "OLT1T0F" ]; then
  BOARD=$1
  CPU=katanaarm
  
  export COMPILER_DIR=/opt/broadcom_kt2_hx4/usr/bin
  export COMPILER_PREFIX=arm-linux-
  export LD_LIB_PATH=/opt/broadcom_kt2_hx4/usr/lib

  # Overide local variables with the ones comming from the makefile (if defined)
  export COMPILER_DIR="${TOOLCHAIN_BIN_DIR:-$COMPILER_DIR}"
  PREFIX=`echo $COMPILER | awk -F'/' '{print $NF}'`
  if [ ! -z $PREFIX ]; then export COMPILER_PREFIX=$PREFIX; fi
  export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-$LD_LIB_PATH}"

  export PTIN_MGMD_PLATFORM_MAX_CHANNELS=4096
  export PTIN_MGMD_PLATFORM_MAX_WHITELIST=16384
  export PTIN_MGMD_PLATFORM_MAX_CLIENTS=512
  export PTIN_MGMD_PLATFORM_MAX_PORTS=12
  export PTIN_MGMD_PLATFORM_MAX_PORT_ID=54
  export PTIN_MGMD_PLATFORM_MAX_SERVICES=40
  export PTIN_MGMD_PLATFORM_MAX_SERVICE_ID=255
  export PTIN_MGMD_PLATFORM_ADMISSION_CONTROL_SUPPORT=1
  export PTIN_MGMD_PLATFORM_ROOT_PORT_IS_ON_MAX_PORT_ID=0

elif [ "$1" == "OLT1T0" ]; then
  BOARD=$1
  CPU=helixarm
  
  export COMPILER_DIR=/opt/broadcom/bin
  export COMPILER_PREFIX=arm-linux-
  export LD_LIB_PATH=/opt/broadcom/lib

  # Overide local variables with the ones comming from the makefile (if defined)
  export COMPILER_DIR="${TOOLCHAIN_BIN_DIR:-$COMPILER_DIR}"
  PREFIX=`echo $COMPILER | awk -F'/' '{print $NF}'`
  if [ ! -z $PREFIX ]; then export COMPILER_PREFIX=$PREFIX; fi
  export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-$LD_LIB_PATH}"

  export PTIN_MGMD_PLATFORM_MAX_CHANNELS=4096
  export PTIN_MGMD_PLATFORM_MAX_WHITELIST=16384
  export PTIN_MGMD_PLATFORM_MAX_CLIENTS=512
  export PTIN_MGMD_PLATFORM_MAX_PORTS=12
  export PTIN_MGMD_PLATFORM_MAX_PORT_ID=54
  export PTIN_MGMD_PLATFORM_MAX_SERVICES=40
  export PTIN_MGMD_PLATFORM_MAX_SERVICE_ID=255
  export PTIN_MGMD_PLATFORM_ADMISSION_CONTROL_SUPPORT=1
  export PTIN_MGMD_PLATFORM_ROOT_PORT_IS_ON_MAX_PORT_ID=0

elif [ "$1" == "TT04SXG" ]; then
  BOARD=$1
  CPU=katanaarm
  
  export COMPILER_DIR=/opt/broadcom_kt2_hx4/usr/bin
  export COMPILER_PREFIX=arm-linux-
  export LD_LIB_PATH=/opt/broadcom_kt2_hx4/usr/lib

  # Overide local variables with the ones comming from the makefile (if defined)
  export COMPILER_DIR="${TOOLCHAIN_BIN_DIR:-$COMPILER_DIR}"
  PREFIX=`echo $COMPILER | awk -F'/' '{print $NF}'`
  if [ ! -z $PREFIX ]; then export COMPILER_PREFIX=$PREFIX; fi
  export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-$LD_LIB_PATH}"

  export PTIN_MGMD_PLATFORM_MAX_CHANNELS=4096
  export PTIN_MGMD_PLATFORM_MAX_WHITELIST=16384
  export PTIN_MGMD_PLATFORM_MAX_CLIENTS=512
  export PTIN_MGMD_PLATFORM_MAX_PORTS=4
  export PTIN_MGMD_PLATFORM_MAX_PORT_ID=11
  export PTIN_MGMD_PLATFORM_MAX_SERVICES=40
  export PTIN_MGMD_PLATFORM_MAX_SERVICE_ID=255
  export PTIN_MGMD_PLATFORM_ADMISSION_CONTROL_SUPPORT=1
  export PTIN_MGMD_PLATFORM_ROOT_PORT_IS_ON_MAX_PORT_ID=1

elif [ "$1" == "TA48GE" ]; then
  BOARD=$1
  CPU=e500
  
  export COMPILER_DIR=/opt/eldk/usr/bin
  export COMPILER_PREFIX=ppc_85xxDP-
#  export LD_LIB_PATH=/opt/broadcom_kt2_hx4/usr/lib

  # Overide local variables with the ones comming from the makefile (if defined)
  export COMPILER_DIR="${TOOLCHAIN_BIN_DIR:-$COMPILER_DIR}"
  PREFIX=`echo $COMPILER | awk -F'/' '{print $NF}'`
  if [ ! -z $PREFIX ]; then export COMPILER_PREFIX=$PREFIX; fi
  export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-$LD_LIB_PATH}"

  export PTIN_MGMD_PLATFORM_MAX_CHANNELS=4096
  export PTIN_MGMD_PLATFORM_MAX_WHITELIST=16384
  export PTIN_MGMD_PLATFORM_MAX_CLIENTS=128
  export PTIN_MGMD_PLATFORM_MAX_PORTS=48
  export PTIN_MGMD_PLATFORM_MAX_PORT_ID=54
  export PTIN_MGMD_PLATFORM_MAX_SERVICES=40
  export PTIN_MGMD_PLATFORM_MAX_SERVICE_ID=255
  export PTIN_MGMD_PLATFORM_ADMISSION_CONTROL_SUPPORT=1
  export PTIN_MGMD_PLATFORM_ROOT_PORT_IS_ON_MAX_PORT_ID=1

elif [ "$1" == "TG16G" ]; then
  BOARD=$1
  CPU=pq2pro
  
  export COMPILER_DIR=/opt/freescale/usr/local/gcc-4.0.2-glibc-2.3.6-nptl-2/powerpc-e300c3-linux/bin
  export COMPILER_PREFIX=powerpc-e300c3-linux-
#  export LD_LIB_PATH=/opt/broadcom_kt2_hx4/usr/lib

  # Overide local variables with the ones comming from the makefile (if defined)
  export COMPILER_DIR="${TOOLCHAIN_BIN_DIR:-$COMPILER_DIR}"
  PREFIX=`echo $COMPILER | awk -F'/' '{print $NF}'`
  if [ ! -z $PREFIX ]; then export COMPILER_PREFIX=$PREFIX; fi
  export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-$LD_LIB_PATH}"

  export PTIN_MGMD_PLATFORM_MAX_CHANNELS=4096
  export PTIN_MGMD_PLATFORM_MAX_WHITELIST=16384
  export PTIN_MGMD_PLATFORM_MAX_CLIENTS=512
  export PTIN_MGMD_PLATFORM_MAX_PORTS=16
  export PTIN_MGMD_PLATFORM_MAX_PORT_ID=22
  export PTIN_MGMD_PLATFORM_MAX_SERVICES=40
  export PTIN_MGMD_PLATFORM_MAX_SERVICE_ID=255
  export PTIN_MGMD_PLATFORM_ADMISSION_CONTROL_SUPPORT=1
  export PTIN_MGMD_PLATFORM_ROOT_PORT_IS_ON_MAX_PORT_ID=1

elif [ "$1" == "CXO160G" ]; then
  BOARD=$1
  CPU=e500mc
  
  export COMPILER_DIR=/opt/fsl/1.2/sysroots/i686-fslsdk-linux/usr/bin/ppce500mc-fsl-linux
  export COMPILER_PREFIX=powerpc-fsl-linux-
#  export LD_LIB_PATH=/opt/broadcom_kt2_hx4/usr/lib

  # Overide local variables with the ones comming from the makefile (if defined)
  export COMPILER_DIR="${TOOLCHAIN_BIN_DIR:-$COMPILER_DIR}"
  PREFIX=`echo $COMPILER | awk -F'/' '{print $NF}'`
  if [ ! -z $PREFIX ]; then export COMPILER_PREFIX=$PREFIX; fi
  export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-$LD_LIB_PATH}"

  export PTIN_MGMD_PLATFORM_MAX_CHANNELS=4096
  export PTIN_MGMD_PLATFORM_MAX_WHITELIST=16384
  export PTIN_MGMD_PLATFORM_MAX_CLIENTS=1
  export PTIN_MGMD_PLATFORM_MAX_PORTS=3
  export PTIN_MGMD_PLATFORM_MAX_PORT_ID=54
  export PTIN_MGMD_PLATFORM_MAX_SERVICES=256
  export PTIN_MGMD_PLATFORM_MAX_SERVICE_ID=255
  export PTIN_MGMD_PLATFORM_ADMISSION_CONTROL_SUPPORT=0
  export PTIN_MGMD_PLATFORM_ROOT_PORT_IS_ON_MAX_PORT_ID=0

elif [ "$1" == "CXO640G" ]; then
  BOARD=$1
  CPU=pq3
  
  export COMPILER_DIR=/opt/eldk/usr/bin
  export COMPILER_PREFIX=ppc_85xxDP-
#  export LD_LIB_PATH=/opt/broadcom_kt2_hx4/usr/lib

  # Overide local variables with the ones comming from the makefile (if defined)
  export COMPILER_DIR="${TOOLCHAIN_BIN_DIR:-$COMPILER_DIR}"
  PREFIX=`echo $COMPILER | awk -F'/' '{print $NF}'`
  if [ ! -z $PREFIX ]; then export COMPILER_PREFIX=$PREFIX; fi
  export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-$LD_LIB_PATH}"

  export PTIN_MGMD_PLATFORM_MAX_CHANNELS=4096
  export PTIN_MGMD_PLATFORM_MAX_WHITELIST=16384
  export PTIN_MGMD_PLATFORM_MAX_CLIENTS=1
  export PTIN_MGMD_PLATFORM_MAX_PORTS=17
  export PTIN_MGMD_PLATFORM_MAX_PORT_ID=101
  export PTIN_MGMD_PLATFORM_MAX_SERVICES=256
  export PTIN_MGMD_PLATFORM_MAX_SERVICE_ID=255
  export PTIN_MGMD_PLATFORM_ADMISSION_CONTROL_SUPPORT=0
  export PTIN_MGMD_PLATFORM_ROOT_PORT_IS_ON_MAX_PORT_ID=0

# Trident3-X3
elif [ "$1" == "TC16SXG" ]; then
  BOARD=$1
  CPU=td3x3arm
  
  export COMPILER_DIR=/opt/xldk/6.0.1/helix5/usr/bin
  export COMPILER_PREFIX=aarch64-linux-
#  export LD_LIB_PATH=/opt/broadcom_kt2_hx4/usr/lib

  # Overide local variables with the ones comming from the makefile (if defined)
  export COMPILER_DIR="${TOOLCHAIN_BIN_DIR:-$COMPILER_DIR}"
  PREFIX=`echo $COMPILER | awk -F'/' '{print $NF}'`
  if [ ! -z $PREFIX ]; then export COMPILER_PREFIX=$PREFIX; fi
  export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-$LD_LIB_PATH}"

  export PTIN_MGMD_PLATFORM_MAX_CHANNELS=4096
  export PTIN_MGMD_PLATFORM_MAX_WHITELIST=16384
  export PTIN_MGMD_PLATFORM_MAX_CLIENTS=512
  export PTIN_MGMD_PLATFORM_MAX_PORTS=16
  export PTIN_MGMD_PLATFORM_MAX_PORT_ID=22
  export PTIN_MGMD_PLATFORM_MAX_SERVICES=40
  export PTIN_MGMD_PLATFORM_MAX_SERVICE_ID=255
  export PTIN_MGMD_PLATFORM_ADMISSION_CONTROL_SUPPORT=1
  export PTIN_MGMD_PLATFORM_ROOT_PORT_IS_ON_MAX_PORT_ID=1

elif [ "$1" == "TG4G" ]; then
  BOARD=$1
  CPU=e500
  
  export COMPILER_DIR=/opt/eldk/usr/bin
  export COMPILER_PREFIX=ppc_85xxDP-
#  export LD_LIB_PATH=/opt/broadcom_kt2_hx4/usr/lib

  # Overide local variables with the ones comming from the makefile (if defined)
  export COMPILER_DIR="${TOOLCHAIN_BIN_DIR:-$COMPILER_DIR}"
  PREFIX=`echo $COMPILER | awk -F'/' '{print $NF}'`
  if [ ! -z $PREFIX ]; then export COMPILER_PREFIX=$PREFIX; fi
  export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-$LD_LIB_PATH}"

  export PTIN_MGMD_PLATFORM_MAX_CHANNELS=1024
  export PTIN_MGMD_PLATFORM_MAX_WHITELIST=4096
  export PTIN_MGMD_PLATFORM_MAX_CLIENTS=512
  export PTIN_MGMD_PLATFORM_MAX_PORTS=4
  export PTIN_MGMD_PLATFORM_MAX_PORT_ID=10
  export PTIN_MGMD_PLATFORM_MAX_SERVICES=40
  export PTIN_MGMD_PLATFORM_MAX_SERVICE_ID=131071
  export PTIN_MGMD_PLATFORM_ADMISSION_CONTROL_SUPPORT=1
  export PTIN_MGMD_PLATFORM_ROOT_PORT_IS_ON_MAX_PORT_ID=1

elif [ "$1" == "TOLT8G" ]; then
  echo "[MGMD] Card $BOARD is deprecated!"
  exit 1

elif [ "$1" == "CXP360G" ]; then
  echo "[MGMD] Card $BOARD is deprecated!"
  exit 1

else
  echo "[MGMD] Card $BOARD is not valid!"
  exit 1
fi

#################### DO NOT CHANGE ANY LINE BELOW! ####################

# Define the main MGMD path (where autogen.sh will generate the configure file)
FP_FOLDER="${FP_FOLDER:-$PWD}"
MGMD_PATH=$FP_FOLDER/src/application/switching/mgmd
MGMD_CONFIGURE=$MGMD_PATH/configure

FP_OUTPUT_PATH=$FP_FOLDER/output/FastPath-Ent-esw-xgs4-$CPU-LR-CSxw-IQH_$BOARD
MGMD_OUTPUT_PATH=$FP_OUTPUT_PATH/objects/mgmd
EXPORT_FILE=$MGMD_OUTPUT_PATH/export.var

readonly LOCKDIR=$FP_FOLDER/.mgmd.lock
readonly LOCK_SLEEP=2

lock() {
  # create lock directory
  echo -n "[MGMD] Locking $LOCKDIR: "
  
  # acquier the lock
  while (! mkdir $LOCKDIR >/dev/null 2>&1); do
    sleep $LOCK_SLEEP;
  done

  # Enable trap to detect CTRL-C or kill commands
  trap "rm -rf ${LOCKDIR}; echo '[MGMD] Lock directory unlocked by trap!'; exit" INT TERM

  echo "done!"
  return 0;
}

unlock() {
  # remove the lock directory
  rm -rf $LOCKDIR >/dev/null 2>&1

  # Disable trap...
  trap "exit" INT TERM

  echo "[MGMD] Unlocking directory $LOCKDIR: done!"
  return 0
}


# Toolchain and SYS_ROOT_DIR definition
export DESTDIR=$MGMD_OUTPUT_PATH/rfs
export SYSROOTDIR=$DESTDIR
export PREFIXDIR=
export ETCDIR=$PREFIXDIR/etc
export PKG_CONFIG_PATH=$DESTDIR/$PREFIXDIR/lib/pkgconfig
#export TARGET_PPC=ppc_85xxDP
#export CROSS_COMPILE=$TARGET_PPC-
export PATH=$PATH:$COMPILER_DIR
#export INCLUDEDIR="/opt/eldk/usr/bin/usr/include"
export CROSSOPTS="--host=ppc-linux --build=$MACHTYPE"
#export CFLAGS="-I$INCLUDEDIR -I$SYSROOTDIR$PREFIXDIR/include"
#export LIBDIR="/opt/ppc-ptin-4.2.2/$TARGET_PPC/usr/lib"
export LIBS=
#export LD_PATH="-L$LIBDIR -L$SYSROOTDIR$PREFIXDIR/lib"
#export LDFLAGS="$LD_PATH"
#export CPPFLAGS="-I$INCLUDEDIR -I$SYSROOTDIR$PREFIXDIR/include"   
export STRIP="$COMPILER_DIR"/"$COMPILER_PREFIX"strip
export CC="$COMPILER_DIR"/"$COMPILER_PREFIX"gcc
export CXX="$COMPILER_DIR"/"$COMPILER_PREFIX"g++
export AR="$COMPILER_DIR"/"$COMPILER_PREFIX"ar
export LD="$COMPILER_DIR"/"$COMPILER_PREFIX"ld
export NM="$COMPILER_DIR"/"$COMPILER_PREFIX"nm
export RANLIB="$COMPILER_DIR"/"$COMPILER_PREFIX"ranlib
export READELF="$COMPILER_DIR"/"$COMPILER_PREFIX"readelf
export OBJCOPY="$COMPILER_DIR"/"$COMPILER_PREFIX"objcopy
export OBJDUMP="$COMPILER_DIR"/"$COMPILER_PREFIX"objdump
export INSTALL=/usr/bin/install

# MGMG specific variables definition
#export PTIN_MGMD_PLATFORM_CTRL_TIMEOUT=
#export PTIN_MGMD_PLATFORM_MSGQUEUE_SIZE=
#export PTIN_MGMD_PLATFORM_STACK_SIZE=
#export PTIN_MGMD_PLATFORM_MAX_FRAME_SIZE=
#export PTIN_MGMD_PLATFORM_SVN_VERSION=
#export PTIN_MGMD_PLATFORM_SVN_RELEASE=
#export PTIN_MGMD_PLATFORM_SVN_PACKAGE=

# Check if clean command is issued
if [ "$2" == "clean" ]; then
  rm -f $MGMD_CONFIGURE
  rm -rf $MGMD_OUTPUT_PATH
  echo "[MGMD] Clean complete!"
  exit 0
fi

# autogen.sh script must be executed once! It must then be protected by a mutex
lock

# Run autogen.sh if 'configure' file does not exist
if [ ! -f $MGMD_CONFIGURE ]; then
  echo "[MGMD] File '$MGMD_CONFIGURE' not found!"
  
  echo "[MGMD] Running autogen.sh from the main MGMD path (protected with a mutex)"
  cd $MGMD_PATH
  sh autogen.sh
  cd - >/dev/null
  
fi

unlock

# Create output path if it doesn't exist
mkdir -pv $MGMD_OUTPUT_PATH

MGMD_REV=`svn info $MGMD_PATH | grep 'Revision' | awk '{print $2}'`
MGMD_CONF_REV=`cat $MGMD_CONFIGURE | grep "PACKAGE_VERSION=" -m 1 | sed "s/[^0-9]*//; s/'//; s/M//" | awk -F. '{print $4}'`

echo -n "[MGMD] Checking revision svn=$MGMD_REV configure=$MGMD_CONF_REV: "
if [ $MGMD_REV != $MGMD_CONF_REV ]; then
  echo "don't match!"
  echo "[MGMD] Running configure..."
  
  cd $MGMD_OUTPUT_PATH
  $MGMD_CONFIGURE --prefix=$PREFIXDIR $CROSSOPTS
  
  if [ $? -ne 0 ]; then
    echo "[MGMD] Error while running configure!"
    cd - >/dev/null
    exit 1
  fi
  
  cd - >/dev/null
else
  echo "match!"
fi

echo -n "[MGMD] Checking Makefile: "
if [ ! -f $MGMD_OUTPUT_PATH/Makefile ]; then
  echo "not found!"
  echo "[MGMD] Running configure..."
  
  cd $MGMD_OUTPUT_PATH
  $MGMD_CONFIGURE --prefix=$PREFIXDIR $CROSSOPTS
  
  if [ $? -ne 0 ]; then
    echo "[MGMD] Error while running configure!"
    cd - >/dev/null
    exit 1
  fi
  
  cd - >/dev/null
else
  echo "found! (skipping configuration)"
fi

echo "[MGMD] Compiling..."
make -j$NUM_CPUS -C $MGMD_OUTPUT_PATH all
#make DESTDIR=$MGMD_OUTPUT_PATH/rfs -C $MGMD_OUTPUT_PATH install

if [ $? -ne 0 ]; then
  echo "[MGMD] Error while compiling!"
  exit 1
fi

echo "[MGMD] Updating binaries and include files..."
board=`echo $BOARD | awk '{print tolower($0)}'`
# Copy defs.h but rename it according to the card name
cp -uv $MGMD_OUTPUT_PATH/src/ptin_mgmd_defs.h $FP_FOLDER/src/l7public/common/ptin/ptin_mgmd_defs_$board.h | awk -F'/' '{if ($NF != "") print $NF " updated!"}' | sed "s/'//"
# Copy other .h files
cp -uv $MGMD_PATH/src/ptin_mgmd_eventqueue.h $FP_FOLDER/src/l7public/common/ptin/ | awk -F'/' '{if ($NF != "") print $NF " updated!"}' | sed "s/'//"
cp -uv $MGMD_PATH/src/ptin_mgmd_api.h $FP_FOLDER/src/l7public/common/ptin/ | awk -F'/' '{if ($NF != "") print $NF " updated!"}' | sed "s/'//"
cp -uv $MGMD_PATH/src/ptin_mgmd_ctrl.h $FP_FOLDER/src/l7public/common/ptin/ | awk -F'/' '{if ($NF != "") print $NF " updated!"}' | sed "s/'//"
# Copy lib
mkdir -pv $FP_OUTPUT_PATH/libs-ptin/mgmd
cp -uv $MGMD_OUTPUT_PATH/src/.libs/libmgmd.a $FP_OUTPUT_PATH/libs-ptin/mgmd/ | awk -F'/' '{if ($NF != "") print $NF " updated!"}' | sed "s/'//"
# Copy cli
mkdir -pv $FP_OUTPUT_PATH/ipl
cp -uv $MGMD_OUTPUT_PATH/src/.libs/mgmd.cli $FP_OUTPUT_PATH/ipl/ | awk -F'/' '{if ($NF != "") print $NF " updated!"}' | sed "s/'//"

echo "[MGMD] Compilation done!"


# Save all variables in export.var file
echo '.EXPORT_ALL_VARIABLES:' > $EXPORT_FILE
echo "export DESTDIR=$DESTDIR" >> $EXPORT_FILE
echo "export SYSROOTDIR=$SYSROOTDIR" >> $EXPORT_FILE
echo "export PREFIXDIR=$PREFIXDIR" >> $EXPORT_FILE
echo "export ETCDIR=$ETCDIR" >> $EXPORT_FILE
echo "export PKG_CONFIG_PATH=$PKG_CONFIG_PATH" >> $EXPORT_FILE
echo "export TARGET_PPC=$TARGET_PPC" >> $EXPORT_FILE
echo "export CROSS_COMPILE=$CROSS_COMPILE" >> $EXPORT_FILE
echo "export COMPILER_DIR=$COMPILER_DIR" >> $EXPORT_FILE
echo "export PATH=$PATH" >> $EXPORT_FILE
echo "export COMPILER_PREFIX=$COMPILER_PREFIX" >> $EXPORT_FILE
echo "export CROSSOPTS=$CROSSOPTS" >> $EXPORT_FILE
echo "export CFLAGS=$CFLAGS" >> $EXPORT_FILE
echo "export LIBDIR=$LIBDIR" >> $EXPORT_FILE
echo "export LIBS=$LIBS" >> $EXPORT_FILE
echo "export LD_PATH=$LD_PATH" >> $EXPORT_FILE
echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH" >> $EXPORT_FILE
echo "export LDFLAGS=$LDFLAGS" >> $EXPORT_FILE
echo "export INCLUDEDIR=$INCLUDEDIR" >> $EXPORT_FILE
echo "export CPPFLAGS=$CPPFLAGS" >> $EXPORT_FILE
echo "export STRIP=$STRIP" >> $EXPORT_FILE
echo "export CC=$CC" >> $EXPORT_FILE
echo "export CXX=$CXX" >> $EXPORT_FILE
echo "export AR=$AR" >> $EXPORT_FILE
echo "export LD=$LD" >> $EXPORT_FILE
echo "export NM=$NM" >> $EXPORT_FILE
echo "export RANLIB=$RANLIB" >> $EXPORT_FILE
echo "export READELF=$READELF" >> $EXPORT_FILE
echo "export OBJCOPY=$OBJCOPY" >> $EXPORT_FILE
echo "export OBJDUMP=$OBJDUMP" >> $EXPORT_FILE
echo "export INSTALL=$INSTALL" >> $EXPORT_FILE
echo "export PTIN_MGMD_PLATFORM_CTRL_TIMEOUT=$PTIN_MGMD_PLATFORM_CTRL_TIMEOUT" >> $EXPORT_FILE   
echo "export PTIN_MGMD_PLATFORM_MSGQUEUE_SIZE=$PTIN_MGMD_PLATFORM_MSGQUEUE_SIZE" >> $EXPORT_FILE
echo "export PTIN_MGMD_PLATFORM_STACK_SIZE=$PTIN_MGMD_PLATFORM_STACK_SIZE" >> $EXPORT_FILE
echo "export PTIN_MGMD_PLATFORM_MAX_CHANNELS=$PTIN_MGMD_PLATFORM_MAX_CHANNELS" >> $EXPORT_FILE
echo "export PTIN_MGMD_PLATFORM_MAX_WHITELIST=$PTIN_MGMD_PLATFORM_MAX_WHITELIST" >> $EXPORT_FILE
echo "export PTIN_MGMD_PLATFORM_MAX_CLIENTS=$PTIN_MGMD_PLATFORM_MAX_CLIENTS" >> $EXPORT_FILE
echo "export PTIN_MGMD_PLATFORM_MAX_PORTS=$PTIN_MGMD_PLATFORM_MAX_PORTS" >> $EXPORT_FILE
echo "export PTIN_MGMD_PLATFORM_MAX_PORT_ID=$PTIN_MGMD_PLATFORM_MAX_PORT_ID" >> $EXPORT_FILE
echo "export PTIN_MGMD_PLATFORM_MAX_SERVICES=$PTIN_MGMD_PLATFORM_MAX_SERVICES" >> $EXPORT_FILE
echo "export PTIN_MGMD_PLATFORM_MAX_SERVICE_ID=$PTIN_MGMD_PLATFORM_MAX_SERVICE_ID" >> $EXPORT_FILE
echo "export PTIN_MGMD_PLATFORM_MAX_FRAME_SIZE=$PTIN_MGMD_PLATFORM_MAX_FRAME_SIZE" >> $EXPORT_FILE
echo "export PTIN_MGMD_PLATFORM_ADMISSION_CONTROL_SUPPORT=$PTIN_MGMD_PLATFORM_ADMISSION_CONTROL_SUPPORT" >> $EXPORT_FILE
echo "export PTIN_MGMD_PLATFORM_ROOT_PORT_IS_ON_MAX_PORT_ID=$PTIN_MGMD_PLATFORM_ROOT_PORT_IS_ON_MAX_PORT_ID" >> $EXPORT_FILE
echo "export PTIN_MGMD_PLATFORM_SVN_RELEASE=$PTIN_MGMD_PLATFORM_SVN_RELEASE" >> $EXPORT_FILE
echo "export PTIN_MGMD_PLATFORM_SVN_VERSION=$PTIN_MGMD_PLATFORM_SVN_VERSION" >> $EXPORT_FILE
echo "export PTIN_MGMD_PLATFORM_SVN_PACKAGE=$PTIN_MGMD_PLATFORM_SVN_PACKAGE" >> $EXPORT_FILE

