#!/bin/sh

if [ $# -eq 0 ]; then
  echo "Syntax: $0 <board> <IPaddr> <slot+1>"
  exit;
fi

IP=$2
DST_PATH=/usr/local/ptin/sbin

USER=root
PASS=lampada

if [ $1 == "ta12xg" ]; then
  output=output/FastPath-Ent-dpp-dnx-e500-LR-CSxw-IQH_TA12XG
elif [ $1 == "tt08sxg" ]; then
  output=output/FastPath-Ent-dpp-dnx-e500mc-LR-CSxw-IQH_TT08SXG
elif [ $1 == "tu100g" ]; then
  output=output/FastPath-Ent-dpp-dnx-e500-LR-CSxw-IQH_TU100G
else
  echo "Invalid board"
  exit;
fi

if [ $# == 2 ]; then
  expect -c "
    spawn telnet $IP
    expect -re \"login:\"
    send \"guest\n\"
    expect -re \"Password:\"
    send \"$PASS\n\"
    expect -re \"]$|]#\"
    send \"su -\n\"
    expect -re \"Password:\"
    send \"$PASS\n\"
    expect -re \"]$|]#\"
    send \"sh /usr/local/ptin/sbin/fastpath stop\n\"
    expect -re \"]$|]#\"
    send \"exit\n\"
    expect -re \"]$|]#\"
    send \"exit\n\"
  "
elif [ $# == 3 ]; then
  expect -c "
    spawn telnet $IP
    expect -re \"login:\"
    send \"guest\n\"
    expect -re \"Password:\"
    send \"$PASS\n\"
    expect -re \"]$|]#\"
    send \"su -\n\"
    expect -re \"Password:\"
    send \"$PASS\n\"
    expect -re \"]$|]#\"
    send \"telnet 192.168.200.$3\n\"
    expect -re \"login:\"
    send \"$USER\n\"
    expect -re \"Password:\"
    send \"$PASS\n\"
    expect -re \"]#\"
    send \"sh /usr/local/ptin/sbin/fastpath stop\n\"
    expect -re \"]#\"
    send \"exit\n\"
    expect -re \"]#\"
    send \"exit\n\"
    expect -re \"]$|]#\"
    send \"exit\n\"
  "
fi

cd $output/ipl
lc_put $2 $3 switchdrvr $DST_PATH
lc_put $2 $3 devshell_symbols.gz $DST_PATH
#lc_put $2 $3 fp.cli $DST_PATH
cd -
