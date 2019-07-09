#!/bin/sh

VERSION="1.0.0"
AUTHOR="Jaime Pereira"
CONTACT="jaime-m-pereira@alticelabs.com"
DATE="06/03/2019"

Reset='\033[0m'           # Text Reset
Red='\033[0;31m'          # Red
Green='\033[0;32m'        # Green
Yellow='\033[0;33m'       # Yellow
Cyan='\033[0;36m'         # Cyan
White='\033[0;37m'        # White

CLEAN=0
HOME=$PWD
LOG="tee -a $HOME/output/output.log"

export TARGET_PREFIX="/opt/broadcom_gh2/usr/bin/arm-linux-"
export CC="${TARGET_PREFIX}gcc"
export CXX="${TARGET_PREFIX}g++"
export AS="${TARGET_PREFIX}as"
export LD="${TARGET_PREFIX}ld"
export STRIP="${TARGET_PREFIX}strip"
export RANLIB="${TARGET_PREFIX}ranlib"
export OBJCOPY="${TARGET_PREFIX}objcopy"
export OBJDUMP="${TARGET_PREFIX}objdump"
export AR="${TARGET_PREFIX}ar"
export NM="${TARGET_PREFIX}nm"
export CROSSOPTS="--host=arm-linux --build=i686-linux"
export CONFIGURE_FLAGS="$CROSSOPTS"
export CFLAGS="-I/opt/broadcom_gh2/usr/include"
export CXXFLAGS=""
export LDFLAGS="-L/opt/broadcom_gh2/usr/lib"
export CPPFLAGS=""
export KCFLAGS=""
export ARCH="arm"
export CROSS_COMPILE=${TARGET_PREFIX}
export CCACHE_DIR=$HOME/.ccache
export LOADADDR=0x61008000

### HELP ###
usage() {
    echo -e "\033[0;36mAlticeLabs linux kernel for AE48GE board v$VERSION - $DATE \033[0m"
    echo -e "$AUTHOR <$CONTACT>"
    echo -e "Usage: $(basename $0)\n"\
        "\t-c         \t- Clean build, othwerwise use existing sources ( linux-4.14.48/)\n"\
        "\t-p <patch> \t- Patch to be used if specified, otherwise use existent on patch folder\n"\
        "\t-j <jobs>  \t- Number of jobs to use on make\n"\
        "\t-h         \t- This help, usage\n" 1>&2; exit 1;
}

### PARSE INPUT OPTIONS ###

while getopts ":p:j:hc" o; do
    case "${o}" in
	p)  if [ ! -z "$p" ]; then
		 echo -e "${Red}Duplicated option -p!!! ${Reset}"; exit 2; fi

	    p=${OPTARG}
            if [[ $p == -* ]]; then
	    	 echo -e "${Red}-p option requires an argument!! ${Reset}"; exit 2; fi ;;
	c) CLEAN=1 ;;

        j)  if [ ! -z "$j" ]; then
                 echo -e "${Red}Duplicated option -j!!! ${Reset}"; exit 2; fi

            j=${OPTARG}
            if [[ $j == -* ]]; then
                 echo -e "${Red}-j option requires an argument!! ${Reset}"; exit 2; fi ;;

	h)  usage ;;
	\?) echo  echo -e "${Red}Invalid option: -$OPTARG ${Reset}" >&2; exit 2 ;;
    esac
done
shift $((OPTIND-1))

if [ -z "${p}" ]; then 
    PATCH=patch/kernel-ae48ge.patch; 
else
    PATCH=$p;
fi

if [ -z "${j}" ]; then
    JOBS=20;
else
    JOBS=$j;
fi


# if not clean build or -c option is missing check if existing sources exist!
if [ $CLEAN -eq 0 ]; then
 [ ! -d "linux-4.14.48" ] && echo -e "${Red}No existing kernel sources! Terminating...${Reset}" && exit 2;
fi


# Do a clean build
if [ $CLEAN -eq 1 ]; then
    echo -e "${Yellow}CLEAN BUILD${Reset}"
    
    echo -e "${Yellow}Checking archive...${Reset}" | ${LOG}
    kernel_file="./dl/linux-4.14.48.tar.xz"
    if [ ! -f "$kernel_file" ]
    then
        echo -e "${Yellow}Getting $kernel_file...${Reset}" | ${LOG}
	# original repo
        # wget http://sources.buildroot.net/linux/linux-4.14.48.tar.xz -P ./dl 2>&1 | ${LOG}
	# alticelabs repo
	wget http://yum.ptin.corppt.com/zrepo/ext/linux-4.14.48.tar.xz -P ./dl 2>&1 | ${LOG}
    fi
    
    [ -d "linux-4.14.48" ] && rm -rf linux-4.14.48 ;
    echo -e "${Yellow}Extracting $kernel_file archive...${Reset}" | ${LOG}
    tar xJf $kernel_file
    echo -e "${Yellow}Applying Broadcom kernel patch from XLDK-5.0.3 ${Reset}" | ${LOG}
    patch -d linux-4.14.48/ -p1 < patch/kernel-4.14.48.patch | ${LOG}
    echo -e "${Yellow}Applying AlticeLabs kernel patch $PATCH ${Reset}" | ${LOG}
    patch -d linux-4.14.48/ -p1 < $PATCH | ${LOG}
else
    echo -e "${Yellow}Using existent sources${Reset}" | ${LOG}
fi

# Make target kernel
    cd  linux-4.14.48/
    make distclean
    make ae48ge_defconfig
    #make menuconfig
    make -j$JOBS uImage
    make dtbs
    cd ..
    cp linux-4.14.48/arch/arm/boot/uImage output/kernel-ae48ge.z
    cp linux-4.14.48/arch/arm/boot/dts/ae48ge.dtb output/

# Clean things ....
rm -rf $CCACHE_DIR .subversion 
