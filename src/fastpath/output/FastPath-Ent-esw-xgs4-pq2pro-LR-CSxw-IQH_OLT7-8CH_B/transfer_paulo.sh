#!/bin/bash

ipaddr=10.112.40.10
equip_ip=10.112.42.233

scr_path=../../../../equipment
cli_path=../../../..
bin_path=./ipl
komodules_path=./target

user=guest
pass=lampada
colorOk="\033[1m\033[32m"
colorKo="\033[1m\033[31m"
colorReset="\033[0m"

# Stopping application in equipment
if [ $1 == "bin" -o $1 == "ko" ]; then
  echo -en $colorOk
  echo "Stopping application in equipment..."
  echo -en $colorReset
  expect -c "
    spawn telnet $equip_ip
    expect \"login: \"
    send \"root\n\"
    expect \"Password: \"
    send \"lampada\n\"
    expect \"# \"
    send \"sh /usr/local/ptin/sbin/fastpath stop\n\"
    expect \"# \"
    send \"exit\n\"
    expect \"# \"
    exit
  "
  echo
fi

# Removing files in Paulo Serverfastpath.insmods
echo -en $colorOk
echo "Cleaning files in Paulo server..."
echo -en $colorReset

expect -c "
  spawn telnet $ipaddr
  expect \"login: \"
  send \"$user\n\"
  expect \"Password: \"
  send \"$pass\n\"
  expect \"$ \"
  send \"rm switchdrvr devshell_symbols.gz *.ko fastpath*\n\"
  expect \"$ \"
  send \"exit\n\"
  expect \"$ \"
  exit
"
echo

# Tranfer fastpath binaries
echo -en $colorOk
echo "Transferring switchdrvr, cli and shell..."
echo -en $colorReset
if [ $1 == "cli" ]; then
  ncftpput -u $user -p $pass -v $ipaddr / $cli_path/fastpath.cli/bin/fastpath.cli $cli_path/fastpath.shell/bin/fastpath.shell
elif [ $1 == "ko" -o $1 == "mod" ]; then
  ncftpput -u $user -p $pass -v $ipaddr / $komodules_path/*.ko
elif [ $1 == "bin" ]; then
  ncftpput -u $user -p $pass -v $ipaddr / $bin_path/switchdrvr $bin_path/devshell_symbols.gz
elif [ $1 == "all" ]; then
  ncftpput -u $user -p $pass -v $ipaddr / $bin_path/switchdrvr $bin_path/devshell_symbols.gz $cli_path/fastpath.cli/bin/fastpath.cli $cli_path/fastpath.shell/bin/fastpath.shell
else
  echo -en $colorKo
  echo "Invalid parameter!"
  echo -en $colorReset
  exit
fi

# Copy files in Paulo's server to equipment
echo -en $colorOk
echo "Copying files to NFS base filesystem..."
echo -en $colorReset
expect -c "
  spawn telnet $ipaddr
  expect \"login: \"
  send \"$user\n\"
  expect \"Password: \"
  send \"$pass\n\"
  expect \"$ \"
  send \"sudo su\n\"
  expect \": \"
  send \"$pass\n\"
  expect \"# \"
  send \"sh transfer_switchdrvr.sh\n\"
  expect \"# \"
  send \"exit\n\"
  expect \"$ \"
  send \"exit\n\"
  exit
"
echo

echo -en $colorOk
echo "Done!"
echo -en $colorReset

exit

# Restarting application
echo -en $colorOk
echo "Restarting application..."
echo -en $colorReset
expect -c "
  spawn telnet $equip_ip
  expect \"login: \"
  send \"root\n\"
  expect \"Password: \"
  send \"lampada\n\"
  expect \"# \"
  send \"sh /usr/local/ptin/sbin/fastpath\n\"
  expect \"# \"
  interact
"

