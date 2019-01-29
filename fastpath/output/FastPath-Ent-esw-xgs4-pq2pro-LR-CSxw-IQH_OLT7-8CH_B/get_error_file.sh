lftp -e "cd /usr/local/ptin/log/fastpath; get error.$2; bye" ftp://root:lampada@$1
mv error.$2 ipl
