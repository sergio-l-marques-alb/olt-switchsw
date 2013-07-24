number_of_args=$#

export KERN_VER=2.6.12.3
export KERNDIR=/home/mruas/ptin/repositorio/PR1003/KERNEL/linux-2.6.12.3-PQ2-256M

export PATH=$PATH:/home/mruas/ptin/repositorio/PR1003/KERNEL/linux-2.6.12.3-PQ2-256M/uboot-tools/
#export LANG=
export OUTPATH=output/FastPath-Ent-esw-xgs4-gto-LR-CSxw-IQH
export FP_VIEWNAME=RELEASE_6.3.0.2-FastPath-Ent-esw-xgs4-gto-LR-CSxw-IQH
export CROSS_COMPILE=/usr/local/pq2-linux-4.0.2/bin/powerpc-e300c3-linux-
export KERNEL_SRC=/home/mruas/ptin/repositorio/PR1003/KERNEL/linux-2.6.12.3-PQ2-256M
export CCVIEWS_HOME=/home/mruas/ptin/repositorio/PR1003/FASTPATH_XGS4/FastPath_6.3.0.2-esw-xgs4-gto

export BIN_BACKUP=../../bin
export BIN_PATH=./ipl
export BIN_FILE=switchdrvr

rm $BIN_PATH/$BIN_FILE
sync

make $1 -j2

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
  if [ $number_of_args -ge 1 ]; then
    echo "Stopping $BIN_FILE in $TARGET_IP"
    (
      sleep 2
      echo "root"
      sleep 1
      echo "lampada"
      sleep 2
      echo "sh /usr/local/ptin/sbin/fastpath stop"
      sleep 2
      echo "exit"
    ) | telnet $1
    echo "Transferring binaries to target $1"
    lftp -e "cd /usr/local/ptin/sbin; put $BIN_PATH/$BIN_FILE; put $BIN_PATH/devshell_symbols.gz; bye" ftp://root:lampada@$1
  fi
fi
echo "Done!!!"
