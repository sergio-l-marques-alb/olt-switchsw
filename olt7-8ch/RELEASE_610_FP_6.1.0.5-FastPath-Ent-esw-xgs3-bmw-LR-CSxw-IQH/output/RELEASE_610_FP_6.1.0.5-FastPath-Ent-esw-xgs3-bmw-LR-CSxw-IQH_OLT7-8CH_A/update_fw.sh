#ip=$1
#login=root
#password=lampada

IMAGE_NFS=/home/mruas/ptin/nfs

#echo "Stopping switchdrvr in $ip"
#    (
#      sleep 2
#      echo "$login"
#      sleep 1
#      echo "$password"
#      sleep 1
#      echo "sh /usr/local/ptin/sbin/fastpath stop"
#      sleep 6
#      echo "exit"
#    ) | telnet $ip

echo "Updating binaries..."
sudo cp ./ipl/switchdrvr $IMAGE_NFS/sbin
sudo cp ./ipl/devshell_symbols.gz $IMAGE_NFS/sbin
sudo cp ../../../fastpath.cli/bin/fastpath.cli $IMAGE_NFS/sbin
sudo cp ../../../fastpath.shell/bin/fastpath.shell $IMAGE_NFS/sbin
sudo sync
echo "Done!"

