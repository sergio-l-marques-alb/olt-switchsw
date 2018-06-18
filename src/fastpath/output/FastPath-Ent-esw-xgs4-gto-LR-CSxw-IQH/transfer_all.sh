HOST_OLT_FW_PATH=~/ptin/repositorio/PR1003/olt7_8ch

#echo "Transferring OLT binaries..."
#cd $HOST_OLT_FW_PATH
#sh transfer_fw.sh $1
#cd -
#echo "   OK"
echo "Transferring Fastpath files..."
sh transfer_fw.sh $1
echo "   OK"
echo "Transferring KO files..."
sh transfer_modules.sh $1
echo "   OK"
echo "Transferring Script files..."
sh transfer_scripts.sh $1
echo "   OK"

