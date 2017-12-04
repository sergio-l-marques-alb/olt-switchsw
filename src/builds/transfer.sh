#!/bin/bash

echo "Syntax: $0 [base_version=3.11] [revision=current] [src_directory=./output]"

base_version=3.11

# Destination
ip_addr=10.112.15.243
username=olt
password=olttera
dst_path="~/switchdrvr_builds"

# Build path (local)
build_path=/home/shared/olt_builds/repository/switchdrv
backup_path=/home/shared/switchdrvr/backup

# Default inputs
svn_rev=`svnversion .. -n | sed -e 's/.*://' -e 's/[A-Z]*$$//'`
src_path=./output

# Inputs
if [ $# -ge 1 ]; then
 base_version=`awk '{print $1"."$2}' FS=. <<< $1`
 echo "base_version=$base_version"
fi
if [ $# -ge 2 ]; then
 svn_rev=$2
fi
if [ $# -ge 3 ]; then
 src_path=$3
fi

echo "Running '$0 $svn_rev $src_path'..."

echo "Transferring files to olt@10.112.15.243:$dst_path..."
lftp -u $username,$password $ip_addr -e "cd $dst_path; mput ${src_path}/fastpath-olt.*-r${svn_rev}.*; exit"

# Only execute @ Kompilovat
if [ "${HOSTNAME%%.*}" == "kompilovat" ]; then
 build_path=$build_path/$base_version
 echo "We are at Kompilovat: transferring image file to build path \"$build_path\"..."
 mkdir -p $build_path
 rm -f $build_path/fastpath-olt.image-* > /dev/null
 cp ${src_path}/fastpath-olt.image-*-r${svn_rev}.tgz $build_path/
 echo "We are at Kompilovat: transferring image file to build path \"$backup_path\"..."

 username=`whoami`
 if [ "$username" == "milton-r-silva" ]; then
  mkdir -p $backup_path
  cp ${src_path}/fastpath-olt.*-r${svn_rev}.* $backup_path/
 fi
fi

#echo "Removing files..."
#rm ${src_path}/fastpath-olt.*r${svn_rev}.*

echo "Done!"
echo "Goto ${username}@${ip_addr}:${dst_path} and run image_update.sh script."
