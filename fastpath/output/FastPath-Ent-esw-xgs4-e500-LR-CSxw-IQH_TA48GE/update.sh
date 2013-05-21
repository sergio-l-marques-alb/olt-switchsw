#!/bin/sh

#lc_put $1 $2 testit /usr/local/ptin/bin
#lc_put $1 $2 go /usr/local/ptin/bin
#lc_put $1 $2 tg16g /usr/local/ptin/sbin

#cd target
#lc_put $1 $2 linux-kernel-bde.ko /usr/local/ptin/sbin
#lc_put $1 $2 linux-user-bde.ko /usr/local/ptin/sbin
#cd -
cd ipl
lc_put $1 $2 switchdrvr /usr/local/ptin/sbin
lc_put $1 $2 devshell_symbols.gz /usr/local/ptin/sbin
cd -
