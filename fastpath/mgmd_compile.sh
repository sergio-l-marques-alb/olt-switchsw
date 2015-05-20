#!/bin/sh

cd src/application/switching/mgmd/
sh build/autogen.sh && sh build/configure-ud.sh && make clean all install
cp rfs/usr/local/ptin/include/mgmd/* ../../../l7public/api/
cd -
echo "Done!"
