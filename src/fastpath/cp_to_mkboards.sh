MKBOARDS=$1
BOARD=$2

if [ $BOARD == "tg16g" ]; then
 echo "Updating TG16G board"
 cd output/FastPath-Ent-esw-xgs4-pq2pro-LR-CSxw-IQH_TG16G
 cp -v ipl/switchdrvr ipl/devshell_symbols.gz target/*.ko ipl/fp.cli ipl/fp.shell $MKBOARDS/TG16G/rootfs/usr/local/ptin/sbin/
 cd -
fi

if [ $BOARD == "cxo160g" ]; then
 echo "Updating CXO160G board"
 cd output/FastPath-Ent-esw-xgs4-e500mc-LR-CSxw-IQH_CXO160G
 cp -v ipl/switchdrvr ipl/devshell_symbols.gz target/*.ko ipl/fp.cli ipl/fp.shell $MKBOARDS/CXO160G/files
 cd -
fi

if [ $BOARD == "cxo640g" ]; then
 echo "Updating CXO640G board"
 cd output/FastPath-Ent-esw-xgs4-pq3-LR-CSxw-IQH_CXO640G
 cp -v ipl/switchdrvr ipl/devshell_symbols.gz target/*.ko ipl/fp.cli ipl/fp.shell $MKBOARDS/CXO640G-MX/rootfs/usr/local/ptin/sbin/
 cd -
fi

if [ $BOARD == "ta48ge" ]; then
 echo "Updating TA48GE board"
 cd output/FastPath-Ent-esw-xgs4-e500-LR-CSxw-IQH_TA48GE
 cp -v ipl/switchdrvr ipl/devshell_symbols.gz target/*.ko ipl/fp.cli ipl/fp.shell $MKBOARDS/TA48GE/rootfs/usr/local/ptin/sbin/
 cd -
fi

echo "$BOARD board updated at $MKBOARDS"

#cp -v ../fastpath.cli/bin/fp.cli ../fastpath.shell/bin/fp.shell $1/TG16G/rootfs/usr/local/ptin/sbin/
#cp -v ../fastpath.cli/bin/fp.cli ../fastpath.shell/bin/fp.shell $1/CXO640G-MX/rootfs/usr/local/ptin/sbin/
#cp -v ../fastpath.cli/bin/fp.cli ../fastpath.shell/bin/fp.shell $1/TA48GE/rootfs/usr/local/ptin/sbin/

echo "DONE!"
