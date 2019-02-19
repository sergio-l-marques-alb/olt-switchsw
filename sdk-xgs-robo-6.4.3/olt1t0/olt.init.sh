#./olt7_init

/bin/sleep 1

fp_status=/usr/local/ptin/bin/fp_status

echo "Waiting for HW to be successfully configured..."
while [ 1 ]
do
  reg=`$fp_status`
#  echo $reg
  
  if [ $reg == "2" ]; then
         echo "Sleeping 5 seconds before OLTD launch..."
         /bin/sleep 5
         echo "Hardware is configured! Launching OLTD..."
         break;
  else
         /bin/sleep 1
  fi
                                 
done


#Agulhar para a PLL
/usr/local/ptin/sbin/spidev_test 0xe0 0xEF 0
/bin/sleep 10


echo "Launching oltd..."
cd /usr/local/ptin/bin
./start_oltd.sh &
cd -
echo "oltd: done!"

