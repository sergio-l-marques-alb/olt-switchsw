ip=$1
login=root
password=lampada

cd ipl
tar czvf fastpath.tgz switchdrvr devshell_symbols.gz
cd -

lftp -e "cd /usr/local/ptin/sbin; put ipl/fastpath.tgz; bye" ftp://$login:$password@$ip

