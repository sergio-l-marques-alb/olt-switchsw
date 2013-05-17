export IMAGE_NFS=/home/mruas/ptin/nfs/sbin

# Copy binaries to image fs
echo "Copying binaries to image fs..."
cp ./ipl/switchdrvr $IMAGE_NFS
cp ./ipl/devshell_symbols.gz $IMAGE_NFS
cp ../../../fastpath.cli/bin/fastpath.cli $IMAGE_NFS
cp ../../../fastpath.shell/bin/fastpath.shell $IMAGE_NFS
sync
echo "Done!"
