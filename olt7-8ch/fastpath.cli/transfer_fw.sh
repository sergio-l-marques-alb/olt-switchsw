#rm unicom_se.tgz
#tar czvf unicom_se.tgz unicom_se
lftp -e "cd /usr/local/ptin/sbin; put bin/fastpath.cli; bye" ftp://root:lampada@$1
