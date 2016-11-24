FASTPATH_ORIGEM=..
FASTPATH_PATH=$FASTPATH_ORIGEM/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH/output/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH_OLT7-8CH_A

ORIGEM_PATH=./apps
BACKUP_PATH=./apps_backup

echo "Cleaning origem..."
cd $ORIGEM_PATH
rm -rf *
cd -

echo "Copying Fastpath files..."
#Updating /var/log/fastpath
cp ./equipment/startup-config $ORIGEM_PATH
#Updating /usr/local/ptin/scripts
cp ./equipment/fastpath.insmods.sh ./equipment/rcS1 $ORIGEM_PATH
#Updating /usr/local/ptin/sbin
cp ./equipment/fastpath ./equipment/fastpath.debugtrace.sh ./equipment/fastpath.debugintercept.sh $ORIGEM_PATH
cp $FASTPATH_PATH/target/linux-kernel-bde.ko $FASTPATH_PATH/target/linux-user-bde.ko $ORIGEM_PATH
cp $FASTPATH_PATH/ipl/switchdrvr $FASTPATH_PATH/ipl/devshell_symbols.gz $ORIGEM_PATH
cp $FASTPATH_PATH/../../../fastpath.cli/bin/fastpath.cli $ORIGEM_PATH
cp $FASTPATH_PATH/../../../fastpath.shell/bin/fastpath.shell $ORIGEM_PATH
cp ./PTin_FASTPATH_ReleaseNotes.txt $ORIGEM_PATH/fastpath.releasenotes
cp $FASTPATH_PATH/ipl/switchdrvr.unstripped $BACKUP_PATH

# Defining the correct permissions
echo "Defining apropriated permissions..."
chmod 666 $ORIGEM_PATH/startup-config
chmod 777 $ORIGEM_PATH/fastpath.insmods.sh
chmod 777 $ORIGEM_PATH/rcS1
chmod 777 $ORIGEM_PATH/fastpath.debugtrace.sh
chmod 777 $ORIGEM_PATH/fastpath.debugintercept.sh
chmod 777 $ORIGEM_PATH/fastpath
chmod 777 $ORIGEM_PATH/fastpath.cli
chmod 777 $ORIGEM_PATH/fastpath.shell
chmod 777 $ORIGEM_PATH/switchdrvr
chmod 666 $ORIGEM_PATH/devshell_symbols.gz
chmod 666 $ORIGEM_PATH/linux-kernel-bde.ko
chmod 666 $ORIGEM_PATH/linux-user-bde.ko
chmod 666 $ORIGEM_PATH/fastpath.releasenotes

echo "DONE!!!"

