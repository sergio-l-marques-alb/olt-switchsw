number_of_args=$#
olt_dir=/home/mruas/repositorio/PR1003

export PATH=$PATH:$olt_dir/kernel/linux-2.6.12.3-PQ2-256M/uboot-tools/
#export LANG=
export OUTPATH=output/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH
export FP_VIEWNAME=RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH
export CROSS_COMPILE=/opt/freescale/usr/local/gcc-4.0.2-glibc-2.3.6-nptl-2/powerpc-e300c3-linux/bin/powerpc-e300c3-linux-
export KERNEL_SRC=$olt_dir/kernel/linux-2.6.12.3-PQ2-256M
export CCVIEWS_HOME=$olt_dir/fastpath

export BIN_BACKUP=../../bin
export BIN_PATH=./ipl
export BIN_FILE=switchdrvr

export IMAGE_NFS=~/ptin/nfs

echo "$PATH"

rm -fv $BIN_PATH/$BIN_FILE
sync

if [ "$1" == "clean" ];then
make clean
exit
fi

if [ "$1" == "cleanall" ];then
make cleanall
exit
fi

# Can be added one module as the input parameter
make $1 -j8

if [ -f $BIN_PATH/$BIN_FILE ]; then
  cd $BIN_PATH
  echo "Saving original $BIN_FILE binary..."
  cp $BIN_FILE $BIN_FILE.unstripped
  echo "Stripping $BIN_FILE binary..."
  "$CROSS_COMPILE"strip $BIN_FILE
  echo "Syncing..."
  sync
  cd -

  #echo "Copying binaries to bin..."
  #cp target/*.ko $BIN_PATH/$BIN_FILE $BIN_BACKUP
#  if [ $number_of_args -ge 1 ]; then
#    echo "Stopping $BIN_FILE in $TARGET_IP"
#    (
#      sleep 2
#      echo "root"
#      sleep 1
#      echo "lampada"
#      sleep 2
#      echo "sh /usr/local/ptin/sbin/fastpath stop"
#      sleep 2
#      echo "exit"
#    ) | telnet $1
#    echo "Transferring binaries to target $1"
#    lftp -e "cd /usr/local/ptin/sbin; put $BIN_PATH/$BIN_FILE; put $BIN_PATH/devshell_symbols.gz; bye" ftp://root:lampada@$1
#  fi
fi
echo "Done!!!"
