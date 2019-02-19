FASTPATH_PATH=.

BIN_PATH=$FASTPATH_PATH/../../../../olt7_8ch/fastpath/bin
BINEXTRA_PATH=$FASTPATH_PATH/../../../../olt7_8ch/fastpath/bin_extra
SOURCE_PATH=$FASTPATH_PATH/../../../../olt7_8ch/fastpath/src
SCRIPTS_PATH=$FASTPATH_PATH/../../../../olt7_8ch/fastpath/scripts
VARLOG_PATH=$FASTPATH_PATH/../../../../olt7_8ch/fastpath/_var_log_fastpath
TRANSFERS_PATH=$FASTPATH_PATH/../../../../olt7_8ch/fastpath/transfers

#Updating SRC files
echo "Copying source file to $SOURCE_PATH"
mv $FASTPATH_PATH/../../../../fp_6.1.0.5_A3.tar.7z $SOURCE_PATH

#Updating BIN files
echo "Copying Binary files to $BIN_PATH"
cp $FASTPATH_PATH/equipment/fastpath $FASTPATH_PATH/equipment/fastpath.debugtrace.sh $FASTPATH_PATH/equipment/fastpath.debugintercept.sh $BIN_PATH
cp $FASTPATH_PATH/target/linux-kernel-bde.ko $FASTPATH_PATH/target/linux-user-bde.ko $BIN_PATH
cp $FASTPATH_PATH/ipl/switchdrvr $FASTPATH_PATH/ipl/devshell_symbols.gz $BIN_PATH
cp $FASTPATH_PATH/../../../fastpath.cli/bin/fastpath.cli $BIN_PATH
cp $FASTPATH_PATH/../../../fastpath.shell/bin/fastpath.shell $BIN_PATH

#Updating BINEXTRA files
echo "Copying Extra Binary files to $BINEXTRA_PATH"
cp $FASTPATH_PATH/ipl/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQHr6v1m0b5.stk $FASTPATH_PATH/ipl/stk_file_vars $BINEXTRA_PATH
cp $FASTPATH_PATH/ipl/switchdrvr.map $FASTPATH_PATH/ipl/switchdrvr.unstripped $BINEXTRA_PATH

#Updating scripts
echo "Copying Script files to $SCRIPTS_PATH"
cp $FASTPATH_PATH/equipment/fastpath.insmods.sh $FASTPATH_PATH/equipment/rcS_ptin $SCRIPTS_PATH

#Updating config files
echo "Copying configuration files to $VARLOG_PATH"
cp $FASTPATH_PATH/equipment/startup-config $FASTPATH_PATH/equipment/startup-config.proxy $VARLOG_PATH

#Updating transferr files
echo "Copying transfer script files to $TRANSFERS_PATH"
cp $FASTPATH_PATH/*.sh $TRANSFERS_PATH

echo "DONE!!!"
