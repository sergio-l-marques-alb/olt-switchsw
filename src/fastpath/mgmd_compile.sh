#!/bin/sh

if [ $# -eq 0 ]; then
 echo "Please, provide target board: CXO640G/CXO160G/TA48GE/TG16G/TG16GF/OLT1T0/OLT1T0F/..."
 exit 0;
fi

BOARD=$1

sh mgmd_config_${BOARD}.sh
cd src/application/switching/mgmd/
sh build/autogen.sh && sh build/configure-mgmd.sh && make clean all install
cp rfs/usr/local/ptin/include/mgmd/* ../../../l7public/api/
cd -
echo "Done!"
