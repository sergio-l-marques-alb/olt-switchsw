mknod /dev/linux-user-bde c 126 0
mknod /dev/linux-kernel-bde c 127 0
insmod linux-kernel-bde.ko dmasize=16M debug=1
insmod linux-user-bde.ko
