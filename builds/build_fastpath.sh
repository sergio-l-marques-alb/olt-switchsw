ORIGEM_PATH=./apps
BACKUP_PATH=./apps_backup
BUILDS_PATH=./versoes/builds
APPS_PATH=./versoes/apps
RELNOTES_PATH=./versoes/ReleaseNotes

n_args=$#

if [ $n_args -lt 1 ]
  then
  echo "Erro de parametros"
  exit;
fi

version=$1
echo "Building fastpath version $version ..."

DATE=`date`

bin=setup_olt7-8ch_fw-fastpath_v$version
origem_tgz=apps_olt7-8ch_fw-fastpath_v$version.tgz
rm -rf $BUILDS_PATH/$bin
rm -rf $APPS_PATH/$origem_tgz
rm -rf $RELNOTES_PATH/PTin_FASTPATH_ReleaseNotes_v$1.txt

echo echo OLT7-8CH Fastpath $version > $ORIGEM_PATH/fastpath.ver
echo echo $DATE >> $ORIGEM_PATH/fastpath.ver
chmod 777 $ORIGEM_PATH/fastpath.ver

if [ -e $ORIGEM_PATH/fastpath.releasenotes ]; then
  echo "=== FASTPATH $version RELEASE NOTES ======================================" > $ORIGEM_PATH/fastpath.releasenotes.txt
  cat $ORIGEM_PATH/fastpath.releasenotes >> $ORIGEM_PATH/fastpath.releasenotes.txt
  echo "==========================================================================" >> $ORIGEM_PATH/fastpath.releasenotes.txt
  echo "Generation date: $DATE" >> $ORIGEM_PATH/fastpath.releasenotes.txt
  rm $ORIGEM_PATH/fastpath.releasenotes
  chmod 666 $ORIGEM_PATH/fastpath.releasenotes.txt
elif [ ! -e $ORIGEM_PATH/fastpath.releasenotes.txt ]; then
  echo "Release Notes file is missing!"
  exit 1;
fi

./mksetup2 setup_olt-8ch_fw_fastpath.sh $ORIGEM_PATH $BUILDS_PATH/$bin

echo "Defining permissions..."
chmod 777 $BUILDS_PATH/$bin

echo "Calculating MD5SUM..."
md5sum $BUILDS_PATH/$bin > $BUILDS_PATH/$bin.md5sum

echo "Making a tarball of the origem files..."
tar czvf $APPS_PATH/$origem_tgz $ORIGEM_PATH $BACKUP_PATH

echo "Copying release notes..."
cp ./PTin_FASTPATH_ReleaseNotes.txt $RELNOTES_PATH/PTin_FASTPATH_ReleaseNotes_v$1.txt
echo "Release built at $DATE" >> $RELNOTES_PATH/PTin_FASTPATH_ReleaseNotes_v$1.txt

echo "Done!"
cat $ORIGEM_PATH/fastpath.releasenotes.txt

