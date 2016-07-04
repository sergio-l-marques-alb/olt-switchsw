#!/bin/sh

echo "Removing modules..."
rmmod linux-user-bde
rmmod linux-kernel-bde

echo "Inserting modules..."
insmod /usr/local/ptin/sbin/linux-kernel-bde.ko dmasize=8M himem=1 forceirq=24 debug=1
insmod /usr/local/ptin/sbin/linux-user-bde.ko

echo "Done!"

#echo "Printing information..."
#cat /proc/bus/pci/devices
#cat /proc/linux-kernel-bde
#cat /proc/linux-user-bde

