SCRIPTS_PATH=/usr/local/ptin/scripts
SBIN_PATH=/usr/local/ptin/sbin
LOG_PATH=/usr/local/ptin/log/fastpath
HOST_PATH=equipment

lftp -e "cd $SCRIPTS_PATH; put $HOST_PATH/fastpath.insmods.sh; put $HOST_PATH/rcS_ptin; cd $SBIN_PATH; put $HOST_PATH/fastpath; put $HOST_PATH/fastpath.debugtrace.sh; put $HOST_PATH/fastpath.debugintercept.sh; cd $LOG_PATH; put $HOST_PATH/startup-config; bye" ftp://root:lampada@$1


