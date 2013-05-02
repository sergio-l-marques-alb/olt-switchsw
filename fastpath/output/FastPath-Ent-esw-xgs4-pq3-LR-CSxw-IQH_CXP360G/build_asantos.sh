#!/bin/bash

###############################################################################
#
# FastPath build file by A.Santos & M.Ruas 2011
#
# NOTE: Change variables COMPILER, KERNEL_PATH and NFS_PATH (optional) in
#       order to adapt this build file to your development environment
#
###############################################################################

# Automatic variables
CPU=`pwd | awk -F'output/' '{print $2}' | awk -F'-' '{print $5}'` # CPU
BOARD=`pwd | awk -F'output/' '{print $2}' | awk -F'IQH_' '{print $2}'`
FP_FOLDER=`pwd | awk -F'/' '{print $(NF-4)}'`     # FastPath base directory
OLT_DIR=`pwd | awk -F '/'$FP_FOLDER '{print $1}'` # Head path of FastPath
USER_NAME=`whoami`

# Configurable variables
NFS_PATH=~/ptin/nfs
case "$CPU" in
    "gto")
        COMPILER=/opt/freescale/usr/local/gcc-4.0.2-glibc-2.3.6-nptl-2/powerpc-e300c3-linux/bin/powerpc-e300c3-linux-
        KERNEL_PATH=$OLT_DIR/kernel/linux-2.6.12.3-PQ2-256M
        ;;
    "pq2pro")
        COMPILER=/opt/freescale/usr/local/gcc-4.0.2-glibc-2.3.6-nptl-2/powerpc-e300c3-linux/bin/powerpc-e300c3-linux-
        KERNEL_PATH=$OLT_DIR/kernel/linux-2.6.38-rc5-denx
        ;;
    "pq3")
        COMPILER=/opt/eldk/usr/bin/ppc_85xxDP-
        KERNEL_PATH=$OLT_DIR/kernel/linux-2.6.27.56
        ;;
    *)
        echo "Platform not supported by this build file: "$CPU
esac


################### DO NOT CHANGE ANYTHING BELOW THIS LINE ####################

CLI_PATH=$OLT_DIR/$FP_FOLDER/fastpath.cli/bin
SHELL_PATH=$OLT_DIR/$FP_FOLDER/fastpath.shell/bin

KO_PATH=./target
BIN_PATH=./ipl
BIN_FILE=switchdrvr

TMP_FILE="/tmp/"$USER_NAME"_fp_compiled_"$FP_FOLDER"_"$CPU"_"$BOARD""

export OUTPATH="output/"`pwd | awk -F'output/' '{print $2}'`
export FP_VIEWNAME=RELEASE_6.3.0.2-FastPath-Ent-esw-xgs4-gto-LR-CSxw-IQH
export CROSS_COMPILE=$COMPILER
export KERNEL_SRC=$KERNEL_PATH
export CCVIEWS_HOME=$OLT_DIR/$FP_FOLDER/FastPath_6.3.0.2-esw-xgs4-gto

N_CPUS=`grep -c 'model name' /proc/cpuinfo`

# Local functions
stamp () {
    date --utc --date "now" +%s
}

stampDiff () {
    diffSec=$(($1-$2))
    if ((diffSec < 0)); then abs=-1; else abs=1; fi
    echo $((diffSec/60*abs))m$(((diffSec*abs)%60))s
}

colorGreen="\033[1m\033[32m"
colorRed="\033[1m\033[31m"
colorYellow="\033[1m\033[33m"
colorReset="\033[0m"

output () {
    case "$2" in
        "green")
            echo -en $colorGreen
            ;;
        "red")
            echo -en $colorRed
            ;;
        "yellow")
            echo -en $colorYellow
            ;;
    esac

    echo "$1"
    echo -en $colorReset
}

rm -fv $BIN_PATH/$BIN_FILE
#sync

if [ "$1" == "clean" ];then
    make clean
    rm -f $TMP_FILE
    exit
fi

if [ "$1" == "cleanall" ];then
    make cleanall
    rm -f $TMP_FILE
    exit
fi

# Can be added one module as the input parameter
if [ "$BOARD" == "" ]; then
    output "Compiling "$FP_FOLDER" for "$CPU" platform" "green"
else
    output "Compiling "$FP_FOLDER" for "$CPU" platform (board: "$BOARD")" "green"
fi
output "Using $N_CPUS CPUs" "green"
T1=`stamp`

# If Fastpath was previously successfuly compiled,
# use a package.cfg without the xweb compilation
if [ -f "$TMP_FILE" ]; then
    output "Replacing package.cfg with the one without xweb and snmp compilation..." "yellow"
    cp package.cfg_woXweb package.cfg
fi

if [ "$1" == "nfs" ]; then
    make -j$N_CPUS
else
    make -j$N_CPUS $1
fi

if [ $? == "0" ]; then
    touch $TMP_FILE
fi
# Replace the original package.cfg
cp package.cfg_original package.cfg

T2=`stamp`
output "Elapsed time: `stampDiff $T1 $T2`" "green"

if [ -f $BIN_PATH/$BIN_FILE ]; then
#    cd $BIN_PATH
    echo "Saving original $BIN_FILE binary..."
    cp $BIN_PATH/$BIN_FILE $BIN_PATH/$BIN_FILE.unstripped
    echo "Stripping $BIN_FILE binary..."
    "$CROSS_COMPILE"strip $BIN_PATH/$BIN_FILE
#    echo "Syncing..."
#    sync
#    cd -

    if [ "$1" == "nfs" ];then
        output "Syncronizing files with '$NFS_PATH' folder..." "green"
        rsync -auhi $BIN_PATH/$BIN_FILE             $NFS_PATH
        rsync -auhi $BIN_PATH/devshell_symbols.gz   $NFS_PATH
        rsync -auhi $KO_PATH/linux-kernel-bde.ko    $NFS_PATH
        rsync -auhi $KO_PATH/linux-user-bde.ko      $NFS_PATH
        rsync -auhi $CLI_PATH/fastpath.cli          $NFS_PATH
        rsync -auhi $SHELL_PATH/fastpath.shell      $NFS_PATH
    else
        output "NFS folder was NOT synchronized! Use '$0 nfs' to include sync" "yellow"
    fi
fi

output "Done!!!" "green"
