mknod /dev/linux-user-bde c 126 0
mknod /dev/linux-kernel-bde c 127 0
insmod linux-kernel-bde.ko dmasize=16M debug=4 usemsi=1 maxpayload=256
insmod linux-user-bde.ko
