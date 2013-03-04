ipaddr=0.0.0.0
option="bin"

scr_path=../../../../equipment
cli_path=../../../..
bin_path=./ipl
komodules_path=./target

user=root
pass=lampada
colorOk="\033[1m\033[32m"
colorKo="\033[1m\033[31m"
colorReset="\033[0m"

error_func() {
  echo -en $colorKo
  echo "Invalid or no parameters!"
  echo "syntax: sh transfer_ncftp <ip-addr> <option>"
  echo "<ip-addr>: IP address of target system"
  echo "<option> :"
  echo " scr => transfer scripts and configuration files"
  echo " cli => transfer cli and shell applications"
  echo " mod => stop, transfer and reload kernel modules"
  echo " bin => stop and transfer fastpath binaries"
  echo " all => execute all previous options"
  echo " 3rd => transfer 3rd party applications"
  echo -en $colorReset
  exit;
}

if [ "$1" == "" ]; then
  error_func;
fi
ipaddr=$1

if [ "$2" != "" ]; then
  option=$2
fi

if [ $option == "bin" -o $option == "mod" -o $option == "all" ]; then
  # Stop fastpath
  echo -en $colorOk
  if [ $option == "bin" ]; then
    echo "Stopping fastpath..."
  elif [ $option == "mod" -o $option == "all" ]; then
    echo "Stopping fastpath and removing kernel modules..."
  fi
  echo -en $colorReset
  (
    sleep 1
    echo "root"
    sleep 1
    echo "lampada"
    sleep 2
    echo "fastpath stop"
    sleep 4
  if [ $option == "mod" -o $option == "all" ]; then
    echo "rmmod linux-user-bde.ko"
    sleep 1
    echo "rmmod linux-kernel-bde.ko"
    sleep 2
  fi
    echo "exit"
  ) | telnet $ipaddr
fi

# Transfer 3rd party applications
if [ $option == "3rd" ]; then
  ncftpput -u $user -p $pass -v $ipaddr /usr/local/ptin/sbin $scr_path/apps/olt7_8ch
  ncftpput -u $user -p $pass -v $ipaddr /usr/local/ptin/sbin/fpgas $scr_path/fpgas/*.fp7

# Transfer files using ncftp
elif [ $option == "scr" ]; then
  echo -en $colorOk
  echo "Transferring scripts and configuration files..."
  echo -en $colorReset
  ncftpput -u $user -p $pass -v $ipaddr /usr/local/ptin/sbin $scr_path/scripts/fastpath $scr_path/scripts/fastpath.debugtrace.sh $scr_path/scripts/fastpath.debugintercept.sh
  ncftpput -u $user -p $pass -v $ipaddr /usr/local/ptin/scripts $scr_path/scripts/rcS0 $scr_path/scripts/rcS1 $scr_path/scripts/rcS_ptin $scr_path/scripts/fastpath.insmods.sh $scr_path/scripts/bashrc_ptin $scr_path/scripts/chmod_files.sh

# Transfer files using ncftp
elif [ $option == "cli" ]; then
  echo -en $colorOk
  echo "Transferring CLI and SHELL applications..."
  echo -en $colorReset
  ncftpput -u $user -p $pass -v $ipaddr /usr/local/ptin/sbin $cli_path/fastpath.cli/bin/fastpath.cli $cli_path/fastpath.shell/bin/fastpath.shell

#Transfer kernel modules
elif [ $option == "mod" ]; then
  echo -en $colorOk
  echo "Transferring kernel modules..."
  echo -en $colorReset
  ncftpput -u $user -p $pass -v $ipaddr /usr/local/ptin/sbin $komodules_path/linux-kernel-bde.ko $komodules_path/linux-user-bde.ko

# Tranfer fastpath binaries
elif [ $option == "bin" ]; then
  echo -en $colorOk
  echo "Transferring switchdrvr..."
  echo -en $colorReset
  ncftpput -u $user -p $pass -v $ipaddr /usr/local/ptin/sbin $bin_path/switchdrvr $bin_path/devshell_symbols.gz

# Tranfer all
elif [ $option == "all" ]; then
  echo -en $colorOk
  echo "Transferring ALL..."
  echo -en $colorReset
  ncftpput -u $user -p $pass -v $ipaddr /usr/local/ptin/scripts $scr_path/scripts/rcS0 $scr_path/scripts/rcS1 $scr_path/scripts/rcS_ptin $scr_path/scripts/fastpath.insmods.sh $scr_path/scripts/bashrc_ptin $scr_path/scripts/chmod_files.sh
  ncftpput -u $user -p $pass -v $ipaddr /usr/local/ptin/sbin \
				   $scr_path/scripts/fastpath $scr_path/scripts/fastpath.debugtrace.sh $scr_path/scripts/fastpath.debugintercept.sh \
				   $cli_path/fastpath.cli/bin/fastpath.cli $cli_path/fastpath.shell/bin/fastpath.shell \
				   $komodules_path/linux-kernel-bde.ko $komodules_path/linux-user-bde.ko \
				   $bin_path/switchdrvr $bin_path/devshell_symbols.gz
else
  error_func;
fi

if [ $option == "mod" -o $option == "all" ]; then
  #Reloading kernel modules
  echo -en $colorOk
  echo "Reloading new kernel modules..."
  echo -en $colorReset
  (
    sleep 1
    echo "root"
    sleep 1
    echo "lampada"
    sleep 2
  if [ $option == "all" ]; then
    echo "sh /usr/local/ptin/scripts/chmod_files.sh"
    sleep 1
  fi
    echo "sh /usr/local/ptin/scripts/fastpath.insmods.sh &"
    sleep 10
    echo "exit"
  ) | telnet $ipaddr
fi

echo -en $colorOk
echo "Done!"
echo -en $colorReset

