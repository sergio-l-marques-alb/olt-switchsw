#!/bin/bash

if [ $# -lt 2 ]; then
 echo "Syntax: $0 <src_directory> <revision>"
 exit 0;
fi

rev=$2

ip_addr=10.112.15.243
username=olt
password=olttera
src_path=$1
dst_path="~/switchdrvr_builds"

echo "Transferring files to olt@10.112.15.243:$dst_path..."
lftp -u $username,$password $ip_addr -e "cd $dst_path; mput $src_path/fastpath-olt.*r$rev.*; exit"

echo "Removing files..."
rm fastpath-olt.*r$rev.*

echo "Done!"
echo "Goto ${username}@${ip_addr}:${dst_path} and run image_update.sgh script."
