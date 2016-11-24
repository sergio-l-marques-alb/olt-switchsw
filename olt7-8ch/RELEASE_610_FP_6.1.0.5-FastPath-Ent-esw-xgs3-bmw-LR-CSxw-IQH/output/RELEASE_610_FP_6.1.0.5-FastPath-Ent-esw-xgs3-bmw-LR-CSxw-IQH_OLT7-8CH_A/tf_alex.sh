user=root
pass=lampada
colorOk="\033[1m\033[32m"
colorKo="\033[1m\033[31m"
colorReset="\033[0m"

if [ "$1" == "" ]; then
    echo -en $colorKo
    echo "Please supply remote IP address!"
    echo -en $colorReset
    exit
fi

if [ "$2" == "mod" ]; then
    echo -en $colorOk
    echo "Stopping switchdrvr and kernel modules in $1"
    echo -en $colorReset
else
    echo -en $colorKo
    echo "Please append 'mod' to the command line to include transfer of kernel modules!"
    echo
    echo -en $colorOk
    echo "Stopping switchdrvr in $1"
    echo -en $colorReset
fi

    (
      sleep 1
      echo "root"
      sleep 1
      echo "lampada"
      sleep 1
      echo "/usr/local/ptin/sbin/fastpath stop"
      sleep 4
if [ "$2" == "mod" ]; then
      echo "rmmod linux-user-bde.ko"
      sleep 1
      echo "rmmod linux-kernel-bde.ko"
      sleep 2
fi
      echo "exit"
    ) | telnet $1

# Transfer files using ncftp
echo -en $colorOk
echo "Transfering switchdrvr..."
echo -en $colorReset
ncftpput -u $user -p $pass -v $1 /usr/local/ptin/sbin ipl/switchdrvr ipl/devshell_symbols.gz

if [ "$2" == "mod" ]; then
    echo -en $colorOk
    echo "Transfering kernel modules..."
    echo -en $colorReset
    ncftpput -u $user -p $pass -v $1 target/linux-kernel-bde.ko target/linux-user-bde.ko
fi

# Booting everything up
echo -en $colorOk
echo "Booting everything up again..."
echo -en $colorReset

    (
      sleep 1
      echo "root"
      sleep 1
      echo "lampada"
      sleep 1
if [ "$2" == "mod" ]; then
      echo "/usr/local/ptin/scripts/fastpath.insmods.sh"
      sleep 5
fi
      echo "/usr/local/ptin/sbin/fastpath"
      sleep 10
#      echo "exit"
    ) | telnet $1

