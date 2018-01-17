echo "Stopping switchdrvr in $1"
    (
      sleep 1
      echo "root"
      sleep 1
      echo "lampada"
      sleep 1
      echo "/usr/local/ptin/sbin/fastpath stop"
      sleep 2
      echo "rmmod linux-user-bde.ko"
      sleep 1
      echo "rmmod linux-kernel-bde.ko"
      sleep 2
      echo "exit"
    ) | telnet $1
lftp -e "cd /usr/local/ptin/sbin; put target/linux-kernel-bde.ko; put target/linux-user-bde.ko; bye" ftp://root:lampada@$1
sleep 2
    (
      sleep 1
      echo "root"
      sleep 1
      echo "lampada"
      sleep 1
      echo "/usr/local/ptin/scripts/fastpath.insmods.sh"
      sleep 5
      echo "exit"
    ) | telnet $1

