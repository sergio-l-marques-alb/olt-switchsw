cd output/FastPath-Ent-esw-xgs4-e500mc-LR-CSxw-IQH_CXO640G/ipl/
lc_put $1 switchdrvr /usr/local/ptin/sbin
lc_put $1 devshell_symbols.gz /usr/local/ptin/sbin
cd -

cd output/FastPath-Ent-esw-xgs4-e500mc-LR-CSxw-IQH_CXO640G/target/
lc_put $1 linux-kernel-bde.ko /usr/local/ptin/sbin
lc_put $1 linux-user-bde.ko /usr/local/ptin/sbin
cd -
