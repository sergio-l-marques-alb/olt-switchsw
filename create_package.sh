#!/bin/sh

# These variables are defined by the calling script
#MODULE=fastpath-olt

echo "Creating LOG_FILE..."
# create log file
LOG_FILE=$PWD/create_package.log

echo "-- start script --" > $LOG_FILE
echo "" >> $LOG_FILE


# variable CROSSCOMPILE must be unseted to proceed with compilation
unset CROSS_COMPILE

APPLICATION=fp

BUILD_PATH=./src/builds
BUILDS_FOLDER=./output
APPS_PATH=./apps
TMP_PATH=./tmp

MAIN_BIN=switchdrvr

#backup_7z=$MODULE.backup-v$version-r$svn_rev.7z
#relnotes_file=$MODULE.releasenotes-v$version-r$svn_rev.txt


#Read switchdrv version
file=version.txt
VERSION=$(cat "$file")

echo "Executing make and make install for $DEV_BOARD..." 

cd $BUILD_PATH

svn_rev=`svnversion .. -n | sed -e 's/.*://' -e 's/[A-Z]*$$//'`

MODULE=swdrv-$DEV_BOARD-$VERSION-r$svn_rev

image_tgz=$MODULE.tgz

echo "Compiling fastpath for $DEV_BOARD ..." >> $LOG_FILE
make -C ../fastpath -f swdrv-$DEV_BOARD.make >> $LOG_FILE 2>>$LOG_FILE
if [ $? -ne 0 ]; then
 echo ERROR!!! >> $LOG_FILE
 exit 1;
fi
echo "Copying binaries to apps..." >> $LOG_FILE
make -C ../fastpath -f swdrv-$DEV_BOARD.make install >> $LOG_FILE 2>>$LOG_FILE
if [ $? -ne 0 ]; then
 echo ERROR!!! >> $LOG_FILE
 exit 1;
fi
echo "Done... Going to prepare tarball." >> $LOG_FILE
echo

if [ ! -e $BUILDS_FOLDER ]; then
 mkdir $BUILDS_FOLDER
else
 echo "$BUILDS_FOLDER folder exists" >> $LOG_FILE
fi

echo "Building $MODULE version swtchdrv-$VERSION-r$svn_rev ..." >> $LOG_FILE 

DATE=`date`

# Check for folder existence
if [ ! -e $APPS_PATH ]; then
  echo "$APPS_PATH folder is missing!" >> $LOG_FILE
  exit 1;
fi
if [ ! -e $APPS_PATH/image ]; then
  echo "Missing folders in $APPS_PATH" >> $LOG_FILE
  exit 1;
fi

# create fp.ver
echo echo Modular OLT $MODULE $VERSION-r$svn_rev > ./$APPLICATION.ver 
echo echo $DATE >> ./$APPLICATION.ver 
chmod 777 ./$APPLICATION.ver 

cp ./$APPLICATION.ver $APPS_PATH/image/$DEV_BOARD/usr/local/scripts/swdrv-scripts
echo "echo OLTSWITCH md5sum: "`md5sum $APPS_PATH/image/$DEV_BOARD/usr/local/sbin/$MAIN_BIN | awk '{print $1}'` >> $APPS_PATH/image/$DEV_BOARD/usr/local/scripts/swdrv-scripts/$APPLICATION.ver 

# create tgz file
echo "Preparing tarball for $DEV_BOARD..."
sync
rm -f $BUILDS_FOLDER/$image_tgz
sync
cd $APPS_PATH/image
rm -f *.tgz 
tar czvf $image_tgz $DEV_BOARD  --owner=root --group=root >> $LOG_FILE 
mv $image_tgz $DESTDIR/$image_tgz

#done without errors
echo "" >> $LOG_FILE
echo "-- end scrpit --" >> $LOG_FILE

echo "Tarball of $DEV_BOARD created and moved to $DESTDIR"

exit 0


