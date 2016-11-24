echo "Removing setups in $2"
    (
      sleep 2
      echo "root"
      sleep 1
      echo "lampada"
      sleep 1
      echo "cd /root ; rm setup_olt7-8ch_fw-fastpath_*"
      sleep 2
      echo "exit"
    ) | telnet $2
ncftpput -u root -p lampada -v $2 /root ./versoes/builds/setup_olt7-8ch_fw-fastpath_v$1
#lftp -e "cd /root; put ../../FASTPATH_builds/builds/setup_olt7-8ch_fw-fastpath_v$1; bye" ftp://root:lampada@$2
echo "Done!"

