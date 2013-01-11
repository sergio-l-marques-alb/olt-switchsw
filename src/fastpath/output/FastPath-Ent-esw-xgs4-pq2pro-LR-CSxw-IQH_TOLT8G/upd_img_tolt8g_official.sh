img_gen=olt/images_generator/TOLT8G
platform=TOLT8G

ipaddr=10.112.15.92

cli_path=../../../..
bin_path=./ipl
komodules_path=./target
releasenotes=..

tmp_folder=tmp

#VXCOMP access
user=mruas
pass=mruas
colorOk="\033[1m\033[32m"
colorKo="\033[1m\033[31m"
colorReset="\033[0m"

error_func() {
  echo -en $colorKo
  echo -en $colorReset
  exit;
}

# Transfer files using ncftp
echo -en $colorOk
echo "Transferring CLI and SHELL applications..."
echo -en $colorReset
ncftpput -u $user -p $pass -v $ipaddr /$tmp_folder $cli_path/fastpath.cli/bin/fastpath.cli $cli_path/fastpath.shell/bin/fastpath.shell

#Transfer kernel modules
echo -en $colorOk
echo "Transferring kernel modules..."
echo -en $colorReset
ncftpput -u $user -p $pass -v $ipaddr /$tmp_folder $komodules_path/linux-kernel-bde.ko $komodules_path/linux-user-bde.ko

# Tranfer fastpath binaries
echo -en $colorOk
echo "Transferring switchdrvr and release notes..."
echo -en $colorReset
ncftpput -u $user -p $pass -v $ipaddr /$tmp_folder $bin_path/switchdrvr $bin_path/devshell_symbols.gz $releasenotes/fastpath.ver

echo -en $colorOk
echo "Making image and transferring to final destination..."
echo -en $colorReset

expect -c "
  spawn ssh $user@$ipaddr
  expect \"password: \"
  send \"$pass\n\"
  expect \"$ \"
  send \"cd ~/$tmp_folder\n\"
  expect \"$ \"
  send \"sudo su\n\"
  expect \"${user}: \"
  send \"$pass\n\"
  expect \"# \"
  send \"chown root:root switchdrvr devshell_symbols.gz linux-kernel-bde.ko linux-user-bde.ko fastpath.cli fastpath.shell fastpath.ver\n\"
  expect \"# \"
  send \"chmod a+x switchdrvr fastpath.cli fastpath.shell\n\"
  expect \"# \"
  send \"mv -f switchdrvr devshell_symbols.gz linux-kernel-bde.ko linux-user-bde.ko fastpath.cli fastpath.shell fastpath.ver /home/$img_gen/${platform}_fs/usr/local/ptin/sbin\n\"
  expect \"# \"
  send \"rm -f *\n\"
  expect \"# \"
  send \"exit\n\"
  expect \"$ \"
  send \"exit\n\"
  expect \"$ \"
  exit
"
#  send \"cd /home/$img_gen\n\"
#  expect \"# \"
#  send \"sh build_ramdisk_${platform}.sh\n\"
#  expect \"# \"
#  send \"sh upload2 $1\n\"
#  expect \"# \"

echo -en $colorOk
echo "Done!"
echo -en $colorReset

