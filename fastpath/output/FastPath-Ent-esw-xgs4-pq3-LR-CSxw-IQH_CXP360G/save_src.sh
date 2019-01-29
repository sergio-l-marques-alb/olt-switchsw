FP_DIR_NAME=FASTPATH_XGS4
FP_TOTAL_PATH=~/ptin/repositorio/PR1003

n_args=$#

if [ $n_args -lt 1 ]
  then
  echo "Erro de parametros"
  exit;
fi

version=$1
echo "Saving fastpath sources (version $version) ..."

src_path=/mnt/vxcomp/ptin/FASTPATH_builds/src_xgs4
src=$src_path/src_olt7-8ch_fw-fastpath-xgs4-6.3.0.2-A1_v$version

echo "Building tarball..."
cd $FP_TOTAL_PATH
nice -n 19 tar cvf $src.tar $FP_DIR_NAME --exclude=.git --exclude=linux-2.6.12.3-PQ2-256M* --exclude=u-boot-1.1.6* > /dev/null 2>&1
cd -

echo "... done!"

