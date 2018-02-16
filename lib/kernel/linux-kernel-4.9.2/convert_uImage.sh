mkimage -A arm64 -O linux -T kernel -C gzip -a 0x80080000 -e 0x80080000 -n Linux -d arch/arm64/boot/Image.gz uImage
