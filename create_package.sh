#!/bin/sh

############################################################################################

print_help()
{
  echo ""
  echo "Usage :$APP card_name"
  echo ""
  echo "Examples:"
  echo "  $APP ae48ge				     # build and create packages for fastpath"
  echo ""
  echo "card_name:"
  echo "  ae48ge ag16ga          # available card names"
  echo ""
}

############################################################################################

# These variables are defined by the calling script
#MODULE=fastpath-olt

echo "Creating LOG_FILE..."
# create log file
LOG_FILE=$PWD/create_package.log

echo "-- start script --" > $LOG_FILE
echo "" >> $LOG_FILE

APPLICATION=swdrv

BUILD_PATH=./src/builds
BUILDS_FOLDER=./output
APPS_PATH=./apps
TMP_PATH=./tmp

MAIN_BIN=switchdrvr

DEV_BOARD_SW="$(echo $DEV_BOARD | awk -F '-' '{print $1}')"

#CLI_SWDRV_NODE_PATH=swdrv.cli/src


export DESTDIR_SWDRV=$BUILD_PATH/$APPS_PATH

# If one parameter is given, use as the board name
if [ $# -ge 1 ]; then
# DEV_BOARD_SW=$1
 DEV_BOARD=$1
 DEV_BOARD_SW="$(echo $1 | awk -F '-' '{print $1}')"
fi

case $DEV_BOARD_SW in
    "AE48GE" | "ae48ge")
      DEV_BOARD_SW="ae48ge"
      ;;
    "AG16GA" | "ag16ga")
      DEV_BOARD_SW="ag16ga"
      ;;
    *)
      log_write "$APP: invalid card -- '$DEV_BOARD_SW'"
      print_help
      exit 1
      ;;
  esac

echo "DEV_BOARD_SW=$DEV_BOARD_SW" >>$LOG_FILE

export DEV_BOARD_SW=$DEV_BOARD_SW

BASE_PATH="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"

# DESTDIR will point to build_dir location (external libs and includes)
if [ -z $DESTDIR ]; then
 DESTDIR=$BASE_PATH/build_dir_local/$DEV_BOARD
fi

# Check if DESTDIR exists
if [ ! -d $DESTDIR ]; then
 echo "ERROR!!! $DESTDIR don't exist!" >> $LOG_FILE
 mkdir -p $DESTDIR
# exit 1;
fi
# Create package folder if it doesn't exist
if [ ! -d "$DESTDIR/packages" ]; then
 echo "$DESTDIR/packages does not exist... Creating it." >> $LOG_FILE
 mkdir -p $DESTDIR/packages
fi

#backup_7z=$MODULE.backup-v$version-r$svn_rev.7z
#relnotes_file=$MODULE.releasenotes-v$version-r$svn_rev.txt


#Read switchdrv version
file=version.txt
VERSION=$(cat "$file")

echo "Executing make and make install for $DEV_BOARD_SW..." 

cd $BUILD_PATH

svn_rev=`svnversion .. -n | sed -e 's/.*://' -e 's/[A-Z]*$$//'`

MODULE=swdrv-$DEV_BOARD_SW-$VERSION-r$svn_rev

image_tgz=$MODULE.tgz

echo "Compiling fastpath for $DEV_BOARD_SW ..." >> $LOG_FILE
make -C ../fastpath -f swdrvr-$DEV_BOARD_SW.make >> $LOG_FILE 2>>$LOG_FILE
if [ $? -ne 0 ]; then
 echo "ERROR compiling SwitchDrv!!!" >> $LOG_FILE
 exit 1;
fi
echo "Copying binaries to apps..." >> $LOG_FILE
make -C ../fastpath -f swdrvr-$DEV_BOARD_SW.make install >> $LOG_FILE 2>>$LOG_FILE
if [ $? -ne 0 ]; then
 echo "ERROR copying SwitchDrv files!!!" >> $LOG_FILE
 exit 1;
fi
echo "Done... Going to prepare tarball." >> $LOG_FILE

if [ ! -d $BUILDS_FOLDER ]; then
 mkdir $BUILDS_FOLDER
else
 echo "$BUILDS_FOLDER folder exists" >> $LOG_FILE
fi

echo "Building $MODULE version swtchdrv-$VERSION-r$svn_rev ..." >> $LOG_FILE 

DATE=`date`

# Check for folder existence
if [ ! -d $APPS_PATH ]; then
  echo "$APPS_PATH folder is missing!" >> $LOG_FILE
  exit 1;
fi
if [ ! -d $APPS_PATH/image ]; then
  echo "Missing folders in $APPS_PATH" >> $LOG_FILE
  exit 1;
fi

#if [ ! -d $APPS_PATH/image/AG16GA/rootfs ]; then
#  echo "Missing folders 5 in $APPS_PATH/image/$DEV_BOARD_SW/rootfs" >> $LOG_FILE
#  exit 1;
#fi

# Goto directory with the board files
cd $APPS_PATH/image/$DEV_BOARD_SW/rootfs
rm -f *.tgz

# create path where LOGs will be located
mkdir -pv var/log/switchdrvr/
# Create swdrv.ver with md5sum and version
echo "echo OLTSWITCH md5sum: "`md5sum ./usr/local/ptin/sbin/$MAIN_BIN | awk '{print $1}'` >> ./usr/local/ptin/sbin/$APPLICATION.ver

# create tgz file
echo "Preparing tarball for $DEV_BOARD_SW..."
sync
rm -f $BUILDS_FOLDER/$image_tgz
sync

tar czvf $image_tgz * --owner=root --group=root >> $LOG_FILE 
if [ $? -ne 0 ]; then
 echo "ERROR creating tarball!!!" >> $LOG_FILE
 exit 1;
fi
mv $image_tgz $DESTDIR/packages/$image_tgz
cd - >> $LOG_FILE
echo "Tarball of $DEV_BOARD created and moved to $DESTDIR/packages"

echo "Success!"

#done without errors
echo "" >> $LOG_FILE
echo "-- end script --" >> $LOG_FILE

exit 0

