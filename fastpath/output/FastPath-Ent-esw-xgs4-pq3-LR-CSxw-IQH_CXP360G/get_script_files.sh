SCRIPTS_PATH=/usr/local/ptin/scripts
SBIN_PATH=/usr/local/ptin/sbin
LOG_PATH=/usr/local/ptin/log/fastpath

mkdir equipment
cd equipment
lftp -e "cd $LOG_PATH;
get startup-config;
cd $SCRIPTS_PATH;
get rcS1
get rcS_ptin;
get fastpath.insmods.sh;
cd $SBIN_PATH;
get fastpath;
get fastpath.debugtrace.sh
get fastpath.debugintercept.sh
bye" ftp://root:lampada@$1
cd -

