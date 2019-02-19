VXCOMP_SERVER=10.112.15.243
VXCOMP_FOLDER=/mnt/nfs_vxcomp/olt/olt7-8ch/build-1.12/olt7-8ch.dir

BUILD_ORIGIN=./versoes

cp $BUILD_ORIGIN/builds/setup_olt7-8ch_fw-fastpath_v$1 ./setup_olt7-8ch_fw2_v$1
cp $BUILD_ORIGIN/builds/setup_olt7-8ch_fw-fastpath_v$1.md5sum ./setup_olt7-8ch_fw2_v$1.md5sum
RELNOTES_FILE=$BUILD_ORIGIN/ReleaseNotes/PTin_FASTPATH_ReleaseNotes_v$1.txt

#lftp -e \
#"cd /develop/olt7-8ch/fastpath;
#put ./setup_olt7-8ch_fw2_v$1;
#put ./setup_olt7-8ch_fw2_v$1.md5sum;
#put $RELNOTES_FILE;
#bye" ftp://share:lampada@10.112.40.70
#echo "Transfer to 10.112.40.70 Done!"

echo "Removing setup in vxcomp server..."
    (
      sleep 2
      echo "gpon"
      sleep 1
      echo "gpon2011"
      sleep 1
      echo "rm $VXCOMP_FOLDER/setup_olt7-8ch_fw2_v*"
      sleep 2
      echo "exit"
    ) | telnet $VXCOMP_SERVER
echo "Transferring setup to vxcomp server..."
ncftpput -u gpon -p gpon2011 -v $VXCOMP_SERVER $VXCOMP_FOLDER ./setup_olt7-8ch_fw2_v$1

#lftp -e \
#"cd /olt7-8ch/build/olt7-8ch.dir;
#mrm ./setup_olt7-8ch_fw2_v*;
#put ./setup_olt7-8ch_fw2_v$1;
#bye" ftp://gpon:gpon2011@10.112.15.92
#echo "Transfer to 10.112.15.92 Done!"

#lftp -e \
#"cd /builds/olt7-8ch/fastpath;
#put ./setup_olt7-8ch_fw2_v$1;
#put ./setup_olt7-8ch_fw2_v$1.md5sum;
#put ../../FASTPATH/PTin_FASTPATH_ReleaseNotes_v$1.txt;
#bye" ftp://guest:lampada@10.112.15.92
#echo "Transfer to public 10.112.15.92 Done!"

rm ./setup_olt7-8ch_fw2_v$1 ./setup_olt7-8ch_fw2_v$1.md5sum
echo "Done!"
