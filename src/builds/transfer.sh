#!/bin/bash

if [ $# -eq 0 ]; then
 echo "Syntax: $0 <revision> [src_directory=./output]"
 exit 0;
fi

src_path=./output

rev=$1
if [ $# -ge 2 ]; then
 src_path=$2
fi

ip_addr=10.112.15.243
username=olt
password=olttera
dst_path="~/switchdrvr_builds"

echo "Transferring files to olt@10.112.15.243:$dst_path..."
lftp -u $username,$password $ip_addr -e "cd $dst_path; mput ${src_path}/fastpath-olt.*-r${rev}.*; exit"

echo "Removing files..."
rm ${src_path}/fastpath-olt.*r${rev}.*

echo "Done!"
echo "Goto ${username}@${ip_addr}:${dst_path} and run image_update.sgh script."
