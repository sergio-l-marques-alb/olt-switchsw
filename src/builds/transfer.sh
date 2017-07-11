#!/bin/bash

echo "Syntax: $0 [revision=current] [src_directory=./output]"

# Destination
ip_addr=10.112.15.243
username=olt
password=olttera
dst_path="~/switchdrvr_builds"

# Default inputs
svn_rev=`svnversion .. -n | sed -e 's/.*://' -e 's/[A-Z]*$$//'`
src_path=./output

# Inputs
if [ $# -ge 1 ]; then
 svn_rev=$1
fi
if [ $# -ge 2 ]; then
 src_path=$2
fi

echo "Running '$0 $svn_rev $src_path'..."

echo "Transferring files to olt@10.112.15.243:$dst_path..."
lftp -u $username,$password $ip_addr -e "cd $dst_path; mput ${src_path}/fastpath-olt.*-r${svn_rev}.*; exit"

echo "Removing files..."
rm ${src_path}/fastpath-olt.*r${svn_rev}.*

echo "Done!"
echo "Goto ${username}@${ip_addr}:${dst_path} and run image_update.sgh script."
