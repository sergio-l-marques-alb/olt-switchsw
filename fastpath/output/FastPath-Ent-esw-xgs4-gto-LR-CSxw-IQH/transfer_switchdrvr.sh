ip=$1
login=root
password=lampada

echo "Stopping switchdrvr in $ip"
    (
      sleep 3
      echo "$login"
      sleep 1
      echo "$password"
      sleep 1
      echo "sh /usr/local/ptin/sbin/fastpath stop"
      sleep 10
      echo "exit"
    ) | telnet $ip
lftp -e "cd /usr/local/ptin/sbin; put ipl/switchdrvr; put ipl/devshell_symbols.gz; bye" ftp://$login:$password@$ip

