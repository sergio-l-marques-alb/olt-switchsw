mknod /dev/linux-user-bde c 126 0
mknod /dev/linux-kernel-bde c 127 0
insmod linux-kernel-bde.ko dmasize=8M debug=4
insmod linux-user-bde.ko
