BASE_PATH=../..
FP_SRC_PATH=../../FASTPATH_builds
FP_DST_PATH=/mnt/vxcomp/ptin/FASTPATH_builds
BUILDS_PATH=builds
APPS_PATH=apps
RELNOTES_PATH=ReleaseNotes
SRC_PATH=src
FASTPATH_FOLDER=FASTPATH

n_args=$#

if [ $n_args -lt 1 ]
  then
  echo "Erro de parametros"
  exit;
fi

version=$1

setup_file=setup_olt7-8ch_fw-fastpath_v$version
apps_tgz_file=apps_olt7-8ch_fw-fastpath_v$version.tgz
relnotes_file=PTin_FASTPATH_ReleaseNotes_v$version.txt
src_file=src_olt7-8ch_fw-fastpath-6.1.0.5-A3_v$version

echo "Saving setup and md5sum files..."
cp $FP_SRC_PATH/$BUILDS_PATH/$setup_file $FP_SRC_PATH/$BUILDS_PATH/$setup_file.md5sum $FP_DST_PATH/$BUILDS_PATH

echo "Saving apps files..."
cp $FP_SRC_PATH/$APPS_PATH/$apps_tgz_file $FP_DST_PATH/$APPS_PATH

echo "Saving release notes..."
cp $FP_SRC_PATH/$RELNOTES_PATH/$relnotes_file $FP_DST_PATH/$RELNOTES_PATH

echo "Saving fastpath sources (version $version) ..."
cd $BASE_PATH
nice -n 19 tar cvf $FP_DST_PATH/$SRC_PATH/$src_file.tar $FASTPATH_FOLDER --exclude=.git --exclude=linux-2.6.12.3-PQ2-256M* --exclude=u-boot-1.1.6* > /dev/null 2>&1
cd -

echo "... done!"

