#!/bin/sh

Reset='\033[0m'           # Text Reset
Red='\033[0;31m'          # Red
Green='\033[0;32m'        # Green
Yellow='\033[0;33m'       # Yellow
Cyan='\033[0;36m'         # Cyan
White='\033[0;37m'        # White

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
export LOADADDR=0x61008000


echo -e "${Yellow}Checking archive...${Reset}"
kernel_file="./dl/linux-4.14.48.tar.xz"
if [ ! -f "$kernel_file" ]
then
    echo -e "${Yellow}Getting kernel...${Reset}" | ${LOG}
    wget http://yum.ptin.corppt.com/zrepo/ext/linux-4.14.48.tar.xz  | ${LOG}
fi
[ -d "linux-4.14.48_clean" ] && rm -rf linux-4.14.48_clean ;

echo -e "${Yellow}Extracting linux kernel archive to a clean folder...${Reset}"
mkdir tmp; tar xJf $kernel_file -C tmp/
mv tmp/linux-4.14.48/ linux-4.14.48_clean; rm tmp -rf

echo -e "${Yellow}Applying Broadcom kernel patch from XLDK-5.0.3 ${Reset}"
patch -d linux-4.14.48_clean -p1 < patch/kernel-4.14.48.patch
echo -e "${Yellow}Cleaning working sources ${Reset}"
cd linux-4.14.48
make distclean;
cd ../

echo -e "${Yellow}Creating patch from linux-4.14.48 folder... ${Reset}"
patch_file_name=kernel-ae48ge-`date +%F-%T`.patch
patch_list=kernel-ae48ge-`date +%F-%T`.clist
diff --no-dereference -rupN linux-4.14.48_clean linux-4.14.48 > output/$patch_file_name
diffstat output/$patch_file_name > output/$patch_list
echo -e "${Green}Patch created and saved to output/$patch_file_name ${Reset}"
echo -e "${Green}Change list saved to output/$patch_list ${Reset}"

patch_rev_name=kernel-ae48ge-r`ls output/kernel-ae48ge-r*.patch 2>/dev/null | wc -l`.patch
patch_rev_list=kernel-ae48ge-r`ls output/kernel-ae48ge-r*.clist 2>/dev/null | wc -l`.clist
ln -s $patch_file_name output/$patch_rev_name
ln -s $patch_list output/$patch_rev_list
