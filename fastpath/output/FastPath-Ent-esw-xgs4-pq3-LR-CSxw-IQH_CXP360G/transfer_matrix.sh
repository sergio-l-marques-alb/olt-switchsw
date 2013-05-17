ipaddr=10.112.42.126
slot=1

user=root
pass=lampada

if [ -n "$1" ]; then
  ipaddr=$1
fi

if [ -n "$2" ]; then
  slot=$2
fi

echo "Stopping application..."
expect -c "
      spawn telnet $ipaddr
      expect \"login: \"
      send \"$user\n\"
      expect \"Password: \"
      send \"$pass\n\"
      expect \"# \"

      send \"telnet 192.168.200.$slot\n\"
      expect \"login: \"
      send \"$user\n\"
      expect \"Password: \"
      send \"$pass\n\"
      expect \"# \"
      send \"cd /usr/local/ptin/sbin\n\"
      expect \"# \"
      send \"fastpath stop\n\"
      expect \"# \"
      send \"sleep 2\n\"
      expect \"# \"
      send \"exit\n\"
      expect \"# \"

      send \"exit\n\"
      expect \"# \"
      exit
    "

echo "Transferring binaries..."
cd ipl
lc_put $ipaddr $slot switchdrvr /usr/local/ptin/sbin
lc_put $ipaddr $slot devshell_symbols.gz /usr/local/ptin/sbin
cd ..

echo "Syncing..."
expect -c "
      spawn telnet $ipaddr
      expect \"login: \"
      send \"$user\n\"
      expect \"Password: \"
      send \"$pass\n\"
      expect \"# \"

      send \"telnet 192.168.200.$slot\n\"
      expect \"login: \"
      send \"$user\n\"
      expect \"Password: \"
      send \"$pass\n\"
      expect \"# \"
      send \"sync\n\"
      expect \"# \"
      send \"sleep 2\n\"
      expect \"# \"
      send \"exit\n\"
      expect \"# \"

      send \"exit\n\"
      expect \"# \"
      exit
    "
echo "Done!"
