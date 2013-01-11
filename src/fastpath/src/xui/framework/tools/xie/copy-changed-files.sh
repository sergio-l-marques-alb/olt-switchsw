#!/bin/bash

src=$1
dst=$2

[ -d $src ] || exit 0;
[ -d $dst ] || mkdir -p $dst

echo "Copying XUI Generated Files, that are modified"

src=`cd $src;pwd`
dst=`cd $dst;pwd`

## Remove the files in dst folder that are not present in src folder
cd $dst 2>&1 >/dev/null
files=`ls content/obj.tmp/*.[ch] | grep -v ew_proto_ | xargs`
for i in $files; do
  if [ ! -f $src/$i ]; then
    #echo $i removed
    rm -f $i
  fi
done
cd - 2>&1 >/dev/null

## Modify the web handlers to include page specific ew_proto
function add_ew_proto_inc()
{
  cd $src 2>&1 >/dev/null
  files=`ls content/obj.tmp/_xeweb_*.c | xargs`
  for i in $files; do
    spec=`echo $i | sed 's|.*/_xeweb_\(.*\)\.c|\1|'`
    grep "EW_PROTO_SPLIT" $i >/dev/null 2>&1
    if [ $? -ne 0 ]; then
      echo "#define EW_PROTO_SPLIT 1" > $i.new
      echo "#include \"ew_proto_common.h\"" >> $i.new
      echo "#include \"ew_proto_$spec.h\"" >> $i.new
      cat $i >> $i.new
      mv -f $i.new $i
    fi
  done
  touch content/obj.tmp/ew_proto_run_if.h
  cd - 2>&1 >/dev/null
}
add_ew_proto_inc

## copy the files to dst folder that are different in src folder
cd $src 2>&1 >/dev/null
files=`ls content/obj.tmp/*.[ch] | xargs`
cd - 2>&1 >/dev/null

mkdir -p $dst/content/obj.tmp

cd $dst 2>&1 >/dev/null
for i in $files; do
  diff $src/$i $i >/dev/null 2>&1
  if [ $? -ne 0 ]; then
    #echo $i changed
    cp -f $src/$i $i
  fi
done
cd - 2>&1 >/dev/null

rm -rf $src/content/obj.tmp
rm -rf $dst/content/html
mv -f $src/content/html $dst/content/

