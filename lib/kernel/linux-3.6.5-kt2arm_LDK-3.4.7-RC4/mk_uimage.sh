#!/bin/bash

if [ $1 ]; then 
LDADDR=$1;
else
if [ -z "$LOADADD"R ]; then
LDADDR=$1;
else
LDADDR=$LOADADDR;
fi
fi

echo "Using LOADADDR: $LDADDR.";

echo "./mkimage -A arm -O linux -T kernel -n Image -a $LDADDR -C none -d ./arch/arm/boot/Image ./uImage_flash.img"

./mkimage -A arm -O linux -T kernel -n Image -a $LDADDR -C none -d ./arch/arm/boot/Image ./uImage_flash.img


